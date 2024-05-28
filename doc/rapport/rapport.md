---
colorlinks: true
title: Rapport de stage OS 2024
author: Daniel Cojucari, Leonard Gomez
output: pdf_document
geometry: margin=3cm
---

# Introduction

L'**OS** (correspondant à _Operating System_ en anglais ou _Système
d'exploitation_ en français) est simplement un logiciel informatique bas niveau
qui permet à l'utilisateur d'**exploiter** l'ensemble des composants qui
constituent un ordinateur d'une manière conviviale et pratique, un example
d'**OS** qui est plus convivial (_User Friendly_) serait **Windows** et par
l'opposé un **OS** qui serait plus pratique serait **Linux**.

Un **OS** implique une interface entre l'utilisateur et l'ordinateur, une
gestion de la mémoire, une communication entre le procésseur et les
péripheriques de l'ordinateur (example: _GPU_) et la gestion de
procéssus (example: _Multitasking_).

![](./os.png)

L'objectif de notre stage, au depart, était de construire un **OS** qui puisse
tourner sur un ordinateur avec quelques fonctionnalités basiques comme une
interface graphique, gestion de la mémoire, et un système de fichiers,
mais le temps limité et les enjeux de ces tâches ont fait évoluer
notre objectif dans le développement de  l'**OS**.

# Environnement de développement

Pour tester notre OS on pourrait flasher l'ISO sur une clef USB à chaque
recompilation et le réinstaller sur un PC, mais ce workflow ne permet pas
d'itérer rapidement. Le processus prendrait plusieurs minutes dans le
meilleur des cas.\
De plus il serait impossible de debugger l'OS avec GDB.

Avec une machine virtuelle par contre une seule commande suffit pour recompiler
et redémarrer l'OS très rapidement.
On a donc choisi d'utiliser QEMU pour simuler un PC avec un cpu x86 32 bits.

# Programmer un processeur x86

Un OS fournit une abstraction des composants matériels et leurs spécificités.
L'un de ces composants à abstraire est le CPU. Un CPU fonctionne différemment.
selon son architecture et son design un CPU x86 ne fonctionne pas comme un
CPU RISC-V, qui lui ne fonctionne pas comme un CPU Motorolla 68k.

Pour programmer cette abstraction correctement, il faut comprendre son
fonctionnement.

Comment fonctionne un CPU x86 ?

## Mode d'opération

- **Real mode**

    > C'est le mode par défaut du CPU lorsque le système s'allume ou se réinitialise.
    > Il simule l'exécution d'un CPU 8086 16 bits. Son nom vient du fait que
    > dans ce mode, toutes les adresses mémoires correspondent aux vraies
    > adresses physiques, il n'y a aucune protection de la mémoire.
    > Dans ce mode, nous avons accès à toutes les interruptions de l'IVT
    > configurées par le BIOS et 1MB de RAM.

- **Protected mode**

    > Ce mode passe le CPU en exécution 32 bits. "Protected" parce que des
    > moyens de protection de la mémoire sont mis en place : organisation
    > des segments et restriction d'accès par privilège avec la GDT,
    > mémoire virtuelle et pagination.
    > Comme les registres font maintenant 32 bits chaque processus
    > peut avoir 4GB de mémoire virtuelle.
    > Pour activer ce mode, il faut mettre le bit PE du registre CR0 à 1,
    > initialiser la GDT et désactiver les interruptions temporairement.
    > ```asm
    >    cli            ; désactiver les interruptions
    >    lgdt [gdtr]    ; charger la GDT
    >    mov eax, cr0
    >    or al, 1       ; activer le bit PE
    >    mov cr0, eax
    >```

- **Long mode**

    > Ce mode ressemble beaucoup au mode protégé mais certaines fonctionnalités
    > comme la segmentation sont complètement désactivées en faveur de la
    > pagination et la mémoire virtuelle. De plus, les registres généraux sont
    > étendus à 64 bits et 8 nouveaux registres d'entiers et de vecteurs sont
    > ajoutés (R8-R15, XMM8-XMM15). Les adresses physiques utilisent 40 bits,
    > et les adresses virtuelles 48. Pour passer en long mode, il faut avoir
    > configuré la mémoire virtuelle au préalable.

Nous avons choisi d'utiliser le mode protégé car il nous donne accès au
plus de fonctionnalités sans avoir à configurer des structures de données
complexes que l'on ne peut pas encore tester sans OS.\
De plus, en mode 32 bits, on peut compiler et exécuter un programme C. Je ne pense
pas que les compilateurs modernes puissent générer du code 16 bits.\
Cependant, si l'on continue le développement de l'OS, il serait judicieux de passer en long mode rapidement
et de se débarrasser du code 32 bits car Intel a annoncé en 2023 que
d'ici 2025, ils ne produiront plus de CPUs 32-bits et donc le mode protégé
deviendra bientôt obsolète.

Cette décision implique qu'il faudra utiliser un cross-compiler pour compiler du
code 32 bits sur notre machine.

# Firmware carte mere

Certaines des sections suivantes parleront de concepts dans le contexte d'un système
qui utilise un firmware BIOS (Basic-Input-Output-System) et non pas un firmware
UEFI, qui est plus récent et a pour but de remplacer le BIOS.

Le BIOS est un firmware pour les cartes mère inventé pour le PC IBM et copié
par la suite par d'autres fabricants. Le mot _firm**ware**_ vient de
_hard**ware**_ et _soft**ware**_. Ce sont des mots composés anglais.
Le terme _hardware_ englobe les composants d'un ordinateur (CPU, GPU, souris,
clavier) tandis que _software_ se limite aux programmes informatique abstraits
qui s'exécutent sur l'ordinateur.

Leur suffixe _**ware**_ veut dire "outil" ou "marchandise".\
_Hard_ veut dire dur, donc la traduction littérale de _hardware_ est
"outil dur".\
_Soft_ veut dire mou, donc la traduction littérale de _software_ est
"outil mou". \
Bien évidemment, ces traductions ont peu de sens puisque ces mots sont
censés être interprétés au sens figuré. Comme dit Jean-Jacques Rousseau :

> "Le langage figuré fut le premier à naître, le sens propre fut trouvé en dernier."

De meilleures traductions seraient "outil physique concret" pour _hardware_ et "outil
numérique abstrait" pour _software_. \
_Firmware_ est un entre-deux. Le préfixe "firm" veut dire
"ferme" comme dans "chair ferme".

Le _firmware_ est donc un logiciel très bas niveau installé en usine sur le _hardware_.
"Ferme" parce que très souvent il ne peut pas être modifié, ni remplacé, puisqu'il
est installé sur des puces ROMs (Read-Only Memory). Dernièrement le _firmware_ est
plutôt installé sur de la mémoire flash pour pouvoir faire des mises à jour.
Évidemment, des mesures sont mises en place pour qu'uniquement leurs blobs
propriétaires soient acceptés pendant la mise à jour. Il ne faudrait pas que le
consommateur utilise son ordinateur comme il le souhaite \\s.

![](./bios_chip.jpg)

"Firm" veut aussi dire "entreprise", donc _firmware_ peut aussi être traduit
comme "outil venant d'entreprise" ce qui est une très jolie coïncidence.

Le BIOS donne une interface graphique pour configurer le système mais donne
aussi une API au programmeur pour se servir des fonctionalités des
composants.

Nous avions choisi de faire un bootloader qui utilise cette interface BIOS par
simplicité. Malgré que le BIOS ne soit pas standard et techniquement obsolète,
il necessite aucune configuration pour fonctionner alors que faire un
bootloader avec l'interface UEFI est plus difficile.

# Booting

![](./matryoshka.jpg)

## POST

Au démarrage, le BIOS exécute le POST (Power-On Self-Test). Cette routine va effectuer plusieurs choses :

- Vérifier l'état du CPU et ses registres
- Vérifier l'intégrité du BIOS
- Détecter tous les composants (RAM, contrôleurs, GPU, chipset)
- Vérifier s'ils fonctionnent
- Les initialiser
- **Initialiser le CPU en "real mode"**
- **Détecter quel disque contient l'OS**
- **Préparer l'API des composants**

Nous allons nous intéresser à ces deux dernières étapes.

## MBR (Master Boot Record)

Tout disque correctement partitionné possède une section MBR au début du disque.
Le MBR est situé sur le premier secteur du disque et occupe 512 octets.

Pendant le POST, le BIOS va lire l'octet 510 du MBR et le comparer à la valeur
magique `0x55AA`. Si l'octet correspond au nombre magique, le système comprend
que c'est sur ce disque que l'OS se trouve.

Le BIOS va ensuite nous donner le contrôle en chargeant les 510 premiers octets
du disque à l'adresse mémoire `0x7C00` pour ensuite exécuter les données à cette
adresse.

Ce programme est le "stage 1" du bootloader.

## Stage 1

Voici un exemple simple :

```asm
; bootloader.asm
; Exemple simple
;******************************************

org 0x7c00       ; addresse du programme
bits 16          ; mode 16 bits
start: jmp boot

;; constant and variable definitions
msg db "Welcome to My Operating System!", 0ah, 0dh, 0h

boot:
cli ; pas d'interruptions hardware
cld ; clear direction flag (voire manuel)
hlt ; halt the system

; On a 512 octets disponibles pour ce programme
; remplisons les avec des 0 pour l'instant
times 510 - ($-$$) db 0
dw 0xAA55 ; Nombre magique
```

Ce programme est limité à une taille de 510 octets car sinon il écrasera le
nombre magique. Il faut donc charger la suite de notre bootloader en mémoire et
l'exécuter avant d'atteindre la limite.

Le bootloader est donc composé de deux parties. Le petit stage 1 qui sert
uniquement de bootstrap, et le stage 2.

## Stage 2

Pour charger le stage 2 de notre bootloader, il faut pouvoir lire le programme
sur le disque et le charger en mémoire.

Pour lire à partir d'un disque en "real mode", il faut utiliser l'API fournie
par le BIOS. Cette API est une table de fonctions, plus précisément elle
s'appelle l'IVT (Interrupt vector table). C'est une structure de données propre
au "real mode" qui est liée au CPU directement par le mécanisme d'interruptions.

Chaque entrée fait 4 octets:

+-----------+------------+---+
|  Offset   |  Segment   |   |
+===========+============+===+
|0          |16          | 32|
+-----------+------------+---+

> Segment est un sélecteur de segment mémoire et offset est le décalage dans ce segment.

La table a 256 entrées et fait donc 1024 octets. Elle se situe à l'adresse 0x0,
oui l'adresse NULL est en fait une adresse valide, celle de l'IVT en
l'occurrence.

Pour appeler une fonction dans cette table, il faut simuler une interruption
avec l'instruction `int` mnémonique pour "interrupt". Par exemple, `int 0x08` va
simuler l'interruption 8 et le CPU va alors chercher la fonction à l'offset 8
dans la table (index 7) et l'exécuter.\
En fonction des valeurs dans les registres généraux, les fonctions vont changer
de comportement. Les registres sont alors les paramètres des fonctions.

La référence pour savoir se servir de ces fonctions est le
[Ralph's brown interrupt list](https://www.cs.cmu.edu/~ralf/files.html).

Une fois le stage 2 lu sur le disque, mis en mémoire et exécuté, il faut se
servir de l'IVT pour recueillir des informations importantes sur le système.
Par exemple, le nombre de disques présents, le mode graphique, les paramètres
d'énergie, etc.
Il faut aussi passer en mode protégé (mode 32 bits) pour pouvoir exécuter notre
OS compilé en 32 bits et initialiser d'autres structures du CPU.

Après avoir essayé pendant quelques jours, nous avons décidé de ne pas faire notre
propre bootloader pour pouvoir aller plus loin dans le développement de l'OS et
ne pas passer 4 semaines à écrire un bootloader.

## Multiboot

Le standard multiboot permet à un kernel de communiquer avec un bootloader
multiboot par deux structures de données standard. N'importe qui peut implémenter
un bootloader multiboot. C'est une spécification GNU.

Deux versions du standard existent : multiboot1 et multiboot2. multiboot2 est plus
modulaire et complet mais il est beaucoup plus difficile à utiliser alors nous
avons choisi d'utiliser multiboot1, qui est suffisant pour notre utilisation.

GRUB est l'implémentation la plus connue mais il existe des alternatives plus
légères comme Limine.

Au démarrage, le BIOS va exécuter GRUB comme décrit précédemment et ensuite GRUB
va s'occuper de plusieurs choses.

L'idée générale est que l'OS déclare un "multiboot header" au début de son
exécutable avec des options pour que le bootloader sache comment paramétrer le
système et quelles informations envoyer en réponse.

```C
/* The magic field should contain this. */
#define MULTIBOOT_HEADER_MAGIC                       0x1BADB002

// Align all boot modules on i386 page (4KB) boundaries.
#define MULTIBOOT_HEADER_FLAG_PAGE_ALIGN             1
#define MULTIBOOT_HEADER_FLAG_MEMORY_INFO            1 << 1
// These flags tell the bootloader what parts of the header are being used
#define MULTIBOOT_HEADER_FLAG_VIDEO_MODE             1 << 2
#define MULTIBOOT_HEADER_FLAG_AOUT_KLUDGE            1 << 16

#define MULTIBOOT_HEADER_FLAGS 	MULTIBOOT_HEADER_FLAG_PAGE_ALIGN |\
                                MULTIBOOT_HEADER_FLAG_MEMORY_INFO |\
                                MULTIBOOT_HEADER_FLAG_VIDEO_MODE |\
                                AOUT_KLUDGE

// [[gun::packed]] est un pragma pour informer le compilateur qu'il
// ne faut pas rajouter de "padding" a ce struct pour l'aligner en memoire
// ce pragma est utilise tres souvent en OS dev parce que les structures
// imposees doivent imperativement correspondre aux specifications
struct [[gnu::packed]] multiboot_header
{
	u32 magic;    // = MULTIBOOT_HEADER_MAGIC
	u32 flags;    // = MULTIBOOT_HEADER_FLAGS
	u32 checksum; // The above fields plus this one must equal 0 mod 2^32.

	/* These are only valid if MULTIBOOT_HEADER_FLAG_AOUT_KLUDGE is set. */
	u32 header_address;
	u32 load_address;
	u32 load_end_address;
	u32 bss_end_address;
	u32 entry_address;

	/* These are only valid if MULTIBOOT_HEADER_FLAG_VIDEO_MODE is set. */
	u32 mode_type;
	u32 width;
	u32 height;
	u32 depth;
};
```

Le bootloader va chercher le "multiboot header" dans les 8192 premiers octets de
l'exécutable de l'OS en cherchant une valeur magique et en vérifiant le
checksum.

Pour placer ce header au début de l'exécutable, il faut le déclarer en
assembleur et le linker manuellement.

```asm
.section .multiboot
.align 4
multiboot_header:
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

    .long multiboot_header
    .long _start
    .long _edata
    .long _end
    .long multiboot_entry

    .long 0
    .long 1024
    .long 768
    .long 0
```

Grace a la section `.multiboot` on peut indiquer au linker ou la placer dans
l'executable finale:

```ld
ENTRY(_start)

SECTIONS
{
    // On skip les deux premiers MO de memoire pour ne pas ecraser GRUB
	. = 2M;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}
```

Si tout est OK, le bootloader va parser le reste du header, générer une
structure de réponse avec les informations demandées (résolution, graphisme...)
et mettre dans le registre `ebx` l'adresse de la structure de réponse et dans
`eax` une valeur magique qui indique à notre OS que tout s'est bien passé et
qu'on a booté avec un bootloader multiboot.

```asm
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	// Reset eflags
	pushl $0
	popf

	cli

	pushl %ebx
	pushl %eax
	call kernel_main

	pushl $halt_message
	call puts
loop: hlt
	jmp loop

halt_message:
	.asciz "Halted."
```

# Graphiques

Pour un OS classique l'affichage le plus efficace se fait à l'aide d'un ou des
_drivers_ qui communiquent avec la carte graphique, en effet c'est la manière
dont la plupart des **OS** fonctionnent **Mais** ceci est une taĉhe impossible
dans notre contexte puisqu'il faudrait écrire un driver pour chaque carte graphique
(la plupart étant faites par _Nvidia_) qui sont propriétaires et donc nous avons
pas accès à leur code source.

Donc nous avons décidé d'utiliser des protocoles standard dans le BIOS comme
**VGA**(_Video Graphics Array_) et **VESA**(_Video Electronics Standards
Association_) configurés par les interruptions de notre _Bootloader_.

Pour avoir un mode graphique on utilise le _multiboot_header_, lorque
_mode_type_ est à 0, _Multiboot_ nous donne accès à un mode graphique linèaire
sinon si _mode_type_ est à 1 on obtient un mode graphique texte standard.

Pour définir la résolution on utilise _width_ et _height_, lorsqu'on est en
mode texte ces deux valeurs vont représenter le nombre de colonnes et lignes
réspectivement, avec une unité representant un caractère, en revanche, lorsqu'on
est en mode linéaire, _width_ et _height_ seront en **pixels**.

## Affichage

### VGA Text Mode

C'est le mode graphique par défaut fourni par _Multiboot_
même si _mode_type_ du _multiboot_header_ n'est pas défini.

Dans ce mode l'adresse du _framebuffer_ (standard)  sera `0xb8000`, ceci est
l'adresse à laquelle nous allons écrire des caractères pour les afficher à
l'écran.


Pour représenter des caractères nous utilisons des valeurs 16 bit où les
premiers 8 bits vont représenter le caractère lui-même en ASCII, les prochains
4 bits: la couleur du caractère (16 couleurs standard) et les derniers
4 bits : la couleur du fond (16 couleurs standard), la représentation peut
varier selon les différents "sous-modes" de _VGA textmode_.

+-----------------+------------------+
| Attribute       |Character         |
+=================+==================+
| 7 6 5 4 3 2 1 0 | 7 6 5 4 3 2 1 0  |
+-----------------+------------------+

### VESA VBE

##  Interface

# IO en x86

C'est bien beau d'avoir un CPU qui peut effectuer des milliards d'opérations à
la seconde mais sans moyen de communiquer avec, autant utiliser un boulier.

Comment est-ce que le CPU communique avec le monde externe ?

## Controllers

![](controllers.jpg)

> > Source : "Operating systems - Design and implementation"
> > Andrew S. Tanenbaum et Albert S. Woodhull

Le CPU communique rarement avec les peripheriques directement.
Le CPU est rattache electriquement a pleins de controlleurs qui se chargent de
gerer un type de peripherique. Par exemple le controlleur 8042 se charge de
gerer les peripheriques PS2 (clavier, souris) et le PC speaker. Tandis que le
controlleur USB s'occupe des peripheriques USB.
Pour communiquer avec ces controlleurs les CPU utilise son bus.

Cette collection de controlleurs externes au CPU s'appelle le chipset.


### Chipset
- Northbridge / Southbridge
- SOC

### IO ports
### MMIO
### DMA

### Etude de cas: Gestion d'un clavier PS2
### Polling vs interruptions
### IDT
### GDT
### Driver 8042
### Driver 8259A
