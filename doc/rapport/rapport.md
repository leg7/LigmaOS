---
colorlinks: true
title: Rapport de stage OS 2024
author: Daniel Cojucari, Leonard Gomez
output: pdf_document
geometry: margin=3.5cm
toc: true
---

# Introduction

L'**OS** (correspondant à _Operating System_ en anglais ou _Système
d'exploitation_ en français) est simplement un logiciel informatique bas niveau
qui permet à l'utilisateur d'**exploiter** l'ensemble des composants qui
constituent un ordinateur d'une manière simple et pratique, un example
d'**OS** qui est plus convivial (_User Friendly_) serait **Windows** et par
l'opposé un **OS** qui serait plus pratique serait **Linux**.

Un **OS** implique une interface entre l'utilisateur et l'ordinateur, une
gestion de la mémoire, une communication entre le procésseur et les
péripheriques de l'ordinateur (example: _GPU_) et la gestion des
procéssus (example: _Multitasking_).

![](./os.png)

L'objectif de notre stage, au depart, était de construire un **OS** qui puisse
tourner sur un ordinateur avec quelques fonctionnalités basiques comme une
interface graphique, gestion de la mémoire, et un système de fichiers,
mais le temps limité et les enjeux de ces tâches ont fait évoluer
notre objectif dans le développement de  l'**OS**.

Certaines parties du développement de l'OS ont été omises ou abrégées pour
rendre le rapport plus facile à lire et moins long.

# Environnement de développement

Pour tester notre OS on pourrait flasher l'ISO sur une clef USB à chaque
recompilation et le réinstaller sur un PC, mais ce workflow ne permet pas
d'itérer rapidement. Le processus prendrait plusieurs minutes dans le
meilleur des cas.\
De plus il serait impossible de debugger l'OS avec GDB.

Avec une machine virtuelle par contre une seule commande suffit pour recompiler
et redémarrer l'OS très rapidement.
On a donc choisi d'utiliser QEMU pour simuler un PC avec un CPU x86 32 bits.

Pour rendre le build reproductible on a choisi d'utiliser Nix et GNU Make.

Nix est un langage fonctionnel pour configurer des systèmes Unix. Grâce à Nix,
on peut créer un environnement shell avec toutes les bonnes dépendances sur
n'importe quelle distribution Linux ou macOS.

La compilation sera faite en mode "freestanding" parce que les implémentations
de libc dépendent de l'OS; sur Linux, le libc dépend des syscalls Linux. Il
faudra donc réimplémenter un libc ou porter un libc existant à notre OS.

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
Cependant, si l'on continue le développement de l'OS, il serait judicieux de
passer en long mode rapidement et de se débarrasser du code 32 bits car Intel a
annoncé en 2023 que d'ici 2025, ils ne produiront plus de CPUs 32-bits et par
consequent le mode protégé deviendra bientôt obsolète.

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

// [[gnu::packed]] est un pragma pour informer le compilateur qu'il
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

# Graphismes

Pour un OS classique l'affichage le plus efficace se fait à l'aide d'un ou des
_drivers_ qui communiquent avec la carte graphique, en effet c'est la manière
dont la plupart des OS fonctionnent **Mais** ceci est une taĉhe impossible
dans notre contexte puisqu'il faudrait écrire un driver pour chaque carte
graphique, la plupart étant propriétaires, nous n'avons pas accès à leur code
source.

Donc nous avons décidé d'utiliser des protocoles standard du BIOS comme
**VGA**(_Video Graphics Array_) et **VESA**(_Video Electronics Standards
Association_) configurés par les interruptions de notre _Bootloader_,.

Pour avoir un mode graphique on utilise le _multiboot_header_, lorsque
_mode_type_ est à 0, _Multiboot_ nous donne accès à un mode graphique linèaire
sinon si _mode_type_ est à 1 on obtient un mode graphique texte standard.

Pour définir la résolution on utilise _width_ et _height_, lorsqu'on est en
mode texte ces deux valeurs vont représenter le nombre de colonnes et lignes
réspectivement, avec une unité representant un caractère, en revanche, lorsqu'on
est en mode linéaire, _width_ et _height_ seront représentés en **pixels**.

En revanche, le mode demendé (résolution, texte ou linéaire etc.) n'est pas
toujours celui qui est fourni par _multiboot_, c'est pour cela qu'il faut
vérifier les données de _multiboot_info_ peu importe le mode graphique choisi.

```C
struct [[gnu::packed]] multiboot_info
{   .
	.
	.
	.
	struct [[gnu::packed]] {
		u64 address;
		u32 pitch;
		u32 width;
		u32 height;
		u8 bpp; // bits per pixel
		u8 type;
	} framebuffer;
	.
	.
	.
};
```

## Affichage

### VGA Text Mode

C'est le mode graphique par défaut, fourni par _Multiboot_,
même si _mode_type_ du _multiboot_header_ n'est pas défini.

Dans ce mode l'adresse du _framebuffer_ (standard)  sera `0xb8000`, c'est
l'adresse à laquelle nous allons écrire des caractères pour les afficher à
l'écran.


Pour représenter des caractères nous utilisons deux octets (_uint_8_) où le
premier octet représente le caractère lui-même en ASCII et pour le deuxième octet, les premiers 4 bits: la couleur du caractère (16 couleurs standard) et les derniers
4 bits : la couleur du fond du caractère (16 couleurs standard).

La représentation peut varier selon les différents "sous-modes" de _VGA textmode_.

+-----------------+-----------------------------------------------+-----------------------------+
|                 |Attribute                                      |Character                    |
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+--+--+-----+-----+-----+-----+
|  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
|Background color       |Foreground color                         |Code point                   |
+-----------------------+-----------------------------------------+-----------------------------+

Pour écrire au _framebuffer_ en mode texte, on écrit les 2 octets du carcatère à son adresse (le _Code point_ en premier) avec le décalage nécessaire.

Le début du _framebuffer_ représente le début de l'écran (en haut à gauche) et parcourt ligne par ligne (de gauche à  droite) l'écran, donc **ce mode est linéaire aussi!**, pour écrire à une coordonée (x,y) on utilisera cette formule (à l'aide des données de _multiboot_info.framebuffer_):
```C
void VGA_text_mode_plot_char(u8 x, u8 y, u8 character, u8 attribute) {
    if ( ( x <= width ) && ( y <= height ) ){
        unsigned short offset = y * pitch + x * 2;
        unsigned char far* video_memory = (unsigned char far*)0xb8000;

        video_memory[offset] = character;
        video_memory[offset + 1] = attribute;
    }
}
```
Ici, _pitch_ représente le nombre d'octets par ligne et donc **ce n'est pas la même chose que _width_!**,
si on voulais utiliser width l'_offset_ se calculerait:
```C
unsigned short offset = (y * width + x) * 2;
```
On multiplie par 2 puisqu'un caractère est representé par 2 octets et le type du framebuffer est _unsigned char\*_.

### VESA VBE
Ce mode nous permet d'écrire à l'écran des *pixels*, qui est beaucoup plus intéressant et qui est aussi le mode d'affichage principal de l'OS.

Dans ce mode, l'adresse du _framebuffer_ n'est pas standard mais on peut la retrouver dans _multi_boot_info.framebuffer_.

Pour ce mode nous avons accés aussi à une autre structure dédiée qui contient des informations potentiellement utiles:

```C
    struct [[gnu::packed]] multiboot_info
    {   .
        .
        .
        .
        struct [[gnu::packed]] {
            u32 control_info;
            u32 mode_info;
            u16 mode;
            u16 interface_seg;
            u16 interface_off;
            u16 interface_length;
        } vbe;
        .
        .
        .
    }
```

La valeur d'un _pixel_ est tout simplement sa couleur, elle est representé dans notre "sous-mode" choisi par une valeur _hex_ de 32 bits sous la forme `0x00RRGGBB`, mais cette valeur peut être aussi en 24 bits (`0xRRGGBB`) parmi d'autres, **on peut avoir jusqu'à 16 777 215 couleurs différentes !**

**Remarque**: Pendant le développement de la partie graphique _VBE_, on voudrait savoir si le "sous-mode" _VBE_ demendé est celui fourni, mais on ne peut pas l'afficher sans avoir déja fait la partie graphique donc cela crée une difficulté.

Pour mettre un pixel à l'écran le méthode est quasiment la même que pour _VGA text mode_:
```C
void VESA_VBE_plot_pixel_32bpp(u16 x, u16 y ,u32 pixel){
    if ( ( x >= width ) && ( y >= length) ){
        unsigned short offset = pitch * y + x * (bpp/8);
        unsigned char far* video_memory = address;

        video_memory[offset] = pixel;
    }
}
```
Ici _bpp_ est le nombre de bits par pixels, dans notre "sous-mode" VBE cette valeur est 32 donc on doit aller de 4 en 4 octets dans l'adresse du framebuffer d'où la division par 8.

##  Interface

Le choix de l'interface est plus subjective que le reste du dévelopement de l'OS,
notre interface utilise surtout les fonctionnalités de l'OS que nous avons reussi à implémenter comme le son, l'horloge et affichage graphique, elle ouvre sur un menu qui donne accès à 3 commandes possibles : "Text input", "Display image" et "Piano" où il ya aussi la date et l'heure qui s'affichent.

- "Text input" affiche un "terminal" ou on peut écrire du texte, le supprimmer et lorsque le "terminal" est rempli, on peut continuer à écrire puisque le texte va monter automatiquement, en revanche on ne peut pas récupérer le texte perdu (triste).La police utilisé est une _bitmap_ dans un fichier C retrouvé sur internet (ayant une licence libre).

- "Display image" affichera une image (libre de droits) qui utilise aussi une _bitmap_ C genérée par un logiciel.

- "Piano" mettra un piano digital où on peut jouer 12 notes différentes (allant de _do4_ à _si4_ pour les musiciens).

En effet, on a du utiliser des fichiers C contenant des _bitmap_ pour avoir une écriture et image à l'écran puisque nous avons pas réussi à implanter un système de fichiers dans le temps réparti.

# IO en x86

C'est bien beau d'avoir un CPU qui peut effectuer des milliards d'opérations à
la seconde, mais sans moyen de communiquer avec, autant utiliser un boulier.

Comment est-ce que le CPU communique avec le monde externe ?

## Controllers

![](controllers.jpg)

> > Source : "Operating systems - Design and implementation"
> > Andrew S. Tanenbaum et Albert S. Woodhull

Le CPU communique rarement avec les périphériques directement. Le CPU est
rattaché électriquement à plusieurs contrôleurs qui se chargent de gérer un type
de périphérique. Par exemple, le contrôleur 8042 se charge de gérer les
périphériques PS2 (clavier, souris) et le PC speaker. Tandis que le contrôleur
USB s'occupe des périphériques USB. Pour communiquer avec ces contrôleurs, le
CPU utilise son bus.

Cette collection de contrôleurs externes au CPU s'appelle le chipset.

Comment envoyer un message au contrôleur ?

## IO ports

Le CPU a un espace d'adresses dédié au contrôle du bus. L'IO address space
commence à `0` et va jusqu'à `2^16 - 1`. Ces adresses correspondent à un espace
mémoire sur la carte mère qui joue un rôle de tampon entre le CPU et les
contrôleurs.

Les adresses des contrôleurs sont hardcodées par Intel. Par exemple, pour le
contrôleur clavier/souris/PC-speaker 8042, le port de données est `0x60` et le
port de commande est `0x64`.

Il n'y a pas d'interface C pour accéder à ces ports, il faut alors passer par de
l'assembleur. Pour lire des données du 8042, l'instruction serait

```asm
mov edx, 0x60
in al, dx
```

Pour éviter de faire de l'assembleur, nous avons créé une interface C qui
appelle ces fonctions assembleur. Il faudra déclarer les signatures des
fonctions en C dans un header et les définir en assembleur.

---

> **Declaration C**

```C
#pragma once
#include <library/types.h>

typedef u16 IO_port_8;
typedef u16 IO_port_16;
typedef u16 IO_port_32;

// le pragma cdecl indique au compilateur que ces fonctions gerent leur pile
// elle memes
[[gnu::cdecl]] void x86_32_out_8(const IO_port_8 port, const u8 data);
[[gnu::cdecl]] void x86_32_out_16(const IO_port_16 port, const u16 data);
[[gnu::cdecl]] void x86_32_out_32(const IO_port_32 port, const u32 data);
[[gnu::cdecl]] u8   x86_32_in_8(const IO_port_8 port);
[[gnu::cdecl]] u16  x86_32_in_16(const IO_port_16 port);
[[gnu::cdecl]] u32  x86_32_in_32(const IO_port_32 port);
[[gnu::cdecl]] void x86_panic(void);
```

---

> **Definition assembeur**

```asm
[bits 32]
global x86_32_out_8, x86_32_out_16, x86_32_out_32, x86_32_in_8, x86_32_in_16, x86_32_in_32

section .text

x86_32_out_8:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, al
	ret

x86_32_out_16:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, ax
	ret

x86_32_out_32:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, eax
	ret

x86_32_in_8:
	mov edx, [esp + 4]
	in al, dx
	ret

x86_32_in_16:
	mov edx, [esp + 4]
	in ax, dx
	ret

x86_32_in_32:
	mov edx, [esp + 4]
	in eax, dx
	ret

global x86_panic
x86_panic:
	cli
	hlt
```

---

Le problème de ce moyen de communication est qu'il est très lent pour transférer
de grandes quantités de données. On ne peut transférer qu'une unité par
instruction (8 ou 32 bits selon le port) et ce transfert utilise du temps CPU.

Le DMA est une solution à ce problème mais nous ne l'avons pas implémentée parce
qu'elle devient vraiment importante pour communiquer avec le disque, quand le
débit de données est très important, chose que nous n'avons pas faite.

## Étude de cas : Gestion d'un clavier PS2

![](ps2.jpg)

Pour interagir avec un ordinateur, avoir un clavier est le strict minimum. Les
utilisateurs VIM diraient même que c'est la meilleure manière d'utiliser un
ordinateur.

Le clavier PS2 ne peut pas communiquer directement avec le CPU, il doit passer
par un contrôleur, le 8042 pour les chipsets x86. Il faudra donc programmer un
driver pour le 8042.

### Driver 8042

Je vous épargne les détails compliqués, l'idée est simple. Le 8042 est branché
sur les IO ports `0x60` et `0x64`. Il faut alors consulter la fiche technique du
8042 pour savoir comment communiquer avec via ses ports. Une fois le contrôleur
initialisé comme indiqué dans le manuel, le clavier écrit des scancodes dans le
data port du 8042 (`0x60`). On peut lire ces codes avec notre interface C.

```C
u8 scancode = x86_in_8(0x60);
```

On crée une interface pour lire les données que le contrôleur nous donne et puis le module est terminé.

```C
#pragma once

u8 PS2_8042_data_read(void);
void PS2_8042_initialize(void);
```

### Polling vs interruptions

Pour lire les scancodes lorsque l'utilisateur utilise le clavier, il existe deux
techniques:

- Polling

> > Le polling est une technique où l'on demande de manière répétée au contrôleur
> > s'il a des nouvelles informations. Le polling peut être bloquant,
> > c'est-à-dire qu'il bloque complètement le CPU pendant qu'il poll, ou non
> > bloquant, où il demande à des intervalles de temps.
> > [Cette scène](https://www.youtube.com/watch?v=pRsxDmxA9Qk) de Shrek 2 illustre
> > parfaitement le concept.

- Interruptions

> > Les interruptions sont un mécanisme essentiel au fonctionnement d'un système
> > x86. Cette fonctionnalité est intégrée électroniquement de manière très
> > efficace. Les interruptions sont un concept simple. Quand le contrôleur a de
> > nouvelles données à partager avec le CPU, il l'interrompt pour qu'il s'en
> > occupe. Le CPU est tellement rapide qu'il donne l'illusion à l'utilisateur qu'il
> > traite des tâches en parallèle.
> >
> > Nous avons opté pour les interruptions, bien qu'elles soient beaucoup plus
> > difficiles à implémenter, car elles sont essentielles au fonctionnement d'un OS
> > et bien plus efficaces que le polling en termes de temps CPU.

### Driver PS2

Le driver PS2 est un module distinct du 8042 car le contrôleur PS2 du système
pourrait être une puce différente. Idéalement, nous devrions abstraire le
contrôleur PS2 et utiliser une interface générique pour recevoir des scancodes.
De cette manière, notre driver PS2 pourrait fonctionner avec n'importe quel
contrôleur, rendant notre code plus générique. Nous avons choisi de ne pas
utiliser cette technique ("hardware abstraction layer" HAL) par manque de temps.

Un scancode est une suite d'octets qui identifie la touche appuyée ou relâchée.\
Par exemple, si l'on appuie sur la touche que l'on interprète comme "contrôle
gauche" sur un clavier ISO, le scancode émis est `0x14` et quand la touche est
relâchée, le code prend deux octets : `0xFA 0x14`. \
"Contrôle droit" appuyé serait `0xE0 0x14` et relâché `0xE0 0xFA 0x14`.

Il est important de comprendre que ces scancodes servent uniquement à identifier
les touches et non pas à leur donner une sémantique. Par exemple, dans une
interprétation de clavier ANSI qwerty, le code `0x1C` correspond à la touche qui
porte le label "a", mais dans une interprétation ISO azerty, ce code correspond
à la touche qui porte le label "q".\ Ces codes représentent en fait les
positions des touches.

Cet encodage présente plusieurs problèmes.\
Premièrement, pour savoir si une touche est appuyée, on ne sait pas combien
d'octets il faut lire, soit 1 seule pour contrôle gauche ou jusqu'à 7 pour
"pause" !.\
Deuxièmement, le contrôleur n'envoie qu'un seul octet par interruption, ce qui
fait qu'on perd le contexte de ce qu'on a lu précédemment à chaque interruption.
Donc, si on reçoit une interruption clavier et on lit `0x14`, est-ce qu'on a
reçu "contrôle gauche appuyé" ou bien est-ce que l'interruption précédente était
`0xFA` et ce code correspond alors à "contrôle gauche relâché" (`0xFA 0x14`) ?\
Ou est-ce que l'interruption précédente était en fait `0xE0` et on doit lire "contrôle droit appuyé" ?\
Ou même encore est-ce que les deux dernières interruptions étaient `0xE0 0xFA`,
ce qui correspond à "contrôle droit relâché" ?

De plus, ces codes peuvent changer en fonction du "scancode set" que le clavier
utilise. Il existe 3 sets différents. Le numéro 1 est considéré obsolète,
presque tous les claviers supportent le 2 et le 3 est très peu utilisé. Nous
avons choisi d'implémenter le "scancode set 2" uniquement.

Pour pallier ce problème, il faut traduire ces codes en un nouvel encodage
logique avec un automate fini déterministe. Notre nouvel encodage s'appelle le
"keycode" et tient sur 1 octet.

```C
union keycode
{
	struct {
		u8 row : 3;
		u8 column : 5;
	};
	u8 code;
};
```

Avec cet encodage, on peut représenter des positions sur un clavier qui a au
maximum 7 rangées et 31 colonnes. Suffisant pour représenter toutes les touches
d'un IBM Battleship!

![](ibm-battleship.jpg)

L'automate reglera le probleme de perte de contexte entre chaque interruption.
Voici un diagramme de l'automate.

```{.mermaid format=pdf}
stateDiagram-v2
    classDef final fill:#318CE7

    INITIAL --> RELEASED : Released
    INITIAL --> EXTENDED : Extended
    INITIAL --> KEY_PRESSED : Scancode
    INITIAL --> PAUSE_PRESSED : Pause

    RELEASED --> KEY_RELEASED : Scancode

    KEY_RELEASED --> INITIAL
    KEY_PRESSED -->  INITIAL

    PAUSE_PRESSED --> KEY_PRESSED

    EXTENDED --> EXTENDED_RELEASED : Released
    EXTENDED --> KEY_PRESSED : Scancode_extended
    EXTENDED --> EXTENDED_PRINT_1 : Print_screen_1

    EXTENDED_PRINT_1 --> EXTENDED_PRINT_2 : Extended
    EXTENDED_PRINT_2 --> KEY_PRESSED : Print_screen_2

    EXTENDED_RELEASED --> KEY_RELEASED : Released
    EXTENDED_RELEASED --> EXTENDED_RELEASED_PRINT_1 : Print_screen_1
    EXTENDED_RELEASED_PRINT_1 --> EXTENDED_RELEASED_PRINT_2 : Extend
    EXTENDED_RELEASED_PRINT_2 --> EXTENDED_RELEASED_PRINT_3 : Released
    EXTENDED_RELEASED_PRINT_3 --> KEY_RELEASED : Print_screen_2

    class KEY_PRESSED, KEY_RELEASED final
```

Les transitions sans labels sont gérées dans le contrôle du flux de l'interruption
clavier. (Voir code)

Maintenant, chaque interruption clavier est un pas dans l'automate.
On peut savoir où l'on en est à n'importe quel moment et effectuer la bonne
traduction en keycode. Pour savoir quelles touches sont actives à un instant T,
on crée un tableau de booléens indexé par les keycodes et on le met à jour dans
les états bleus.

```C
keycode azerty_a = { .row = 3, .col = 2 }
bool a_is_pressed = KEY_IS_PRESSED[azerty_a.code];
```

Il ne reste plus qu'à associer une sémantique aux keycodes. Cela se fait
facilement grâce à une table de traduction `keycode_to_key`.

```C
constexpr enum key const keycode_to_key[KEYMAP_ROWS][KEYMAP_COLUMNS] = {
	{ KEY_ESCAPE, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8,   ... },

	{ KEY_BACKTICK, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,  ... },

	{ KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O,       ... },

	{ KEY_CAPS_LOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, ... },

	{ KEY_LEFT_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA,   ... },

	{ KEY_LEFT_CONTROL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_SPACE, KEY_RIGHT_ALT,       ... },
};
```

Une fois tout cela fait, il faut configurer le CPU correctement pour qu'il
appelle notre fonction de transition à chaque interruption clavier. Comment
faire ?

### IDT

Un CPU x86 peut gérer 256 interruptions différentes. Comment est-ce que l'on
peut configurer ce que le CPU fait pour chaque interruption ?

Le CPU a une table qui s'appelle l'IDT (Interrupt Descriptor Table). Cette table
est très similaire à l'IVT du mode réel. L'IVT du mode réel était la table des
handlers d'interruptions fournie par le BIOS comme API pour les composants.\
Lorsque l'on passe en mode 32 bits, on doit utiliser l'IDT.

Quand le CPU reçoit une interruption N, il va lire l'entrée à l'offset N dans
l'IDT, vérifier des bits de permissions, et sauter à l'adresse du handler
configuré dans l'entrée. Avec ce mécanisme, on peut faire une fonction pour
chaque interruption et les enregistrer dans l'IDT.

Un handler d'interruptions s'appelle une "Interrupt Service Routine" (ISR).

---

![](idt-1.png)

---

Cette approche est simple mais le problème est que l'on doit programmer chaque
ISR en assembleur parce qu'il faut sauvegarder l'état du CPU pour pouvoir y
revenir une fois l'interruption traitée.

Nous avions choisi d'implémenter les interruptions en assembleur, mais avons
vite changé d'avis et changé de méthode.

La méthode la plus utilisée pour programmer les ISR en C est la suivante:

1. Chaque ISR appelle la fonction assembleur commune en lui passant son numéro
   d'interruption en paramètre.
2. La fonction commune assembleur va:
    1. Sauvegarder l'état du CPU dans son prologue.
    2. Appeler le dispatcher C en lui passant le numéro de l'interruption à
       traiter, qui va vérifier s'il existe un handler pour l'interruption en C
    et l'appeler sinon afficher un message d'erreur.
    3. Restaurer l'état du CPU dans l'épilogue.

---

![](idt-2.png)

---

De cette manière on peut facilement ajouter et changer les handlers
d'interruptions depuis notre code C. Le seul bémol est que l'on passe par 4
niveaux d'indirection par rapport à un seul avec la méthode assembleur. Spoiler
alert ! cette structure va passer à 5 niveaux d'indirection par la suite. Sans
compter l'accès à la table GDT pour trouver l'ISR en mémoire.

Tout ceci est aberrant d'un point de vue performance. Ce genre de machine de
Rube Goldberg est à éviter au maximum quand une solution plus simple est
disponible. Si vous voulez en apprendre plus sur les dégradations en performance
qu'un système comme celui-ci implique, je recommande
[cette article/vidéo](https://www.computerenhance.com/p/clean-code-horrible-performance).
Malheureusement nous n'avons pas le temps d'implémenter une meilleure solution
par manque de temps.

### GDT

Nous allons aborder le GDT très rapidement pour nous concentrer sur le clavier.

En mode 16 bits les CPUs pouvaient adresser `2^16 - 1` octets de mémoire avec un
registre, ce qui équivaut à 64K. Ça fait vraiment pas beaucoup. Les légendes
racontent que Bill Gates a dit en 1981 que 640K de mémoire devrait être
suffisant pour quiconque. La citation est probablement fausse mais elle montre
bien que 64K ne suffisait même pas à l'époque, et encore moins aujourd'hui où un
"Hello World" en Node.js utilise 500M de RAM, soit 7812 fois plus... La solution
proposée par Intel était de diviser la mémoire en segments. Pour adresser la
mémoire on utilise alors la syntaxe `segment:offset` où segment est un registre
de segment de 4 bits et offset un registre général de 16 bits. Cette technique
permet d'adresser 1M de RAM.

En mode 32 bits, Intel propose le GDT (General Descriptor Table). C'est une
table qui a pour but d'organiser les sélecteurs de segments que le programmeur
utilise. Le programmeur peut alors choisir un segment pour la pile de l'OS qui
nécessite un privilège administrateur et un segment pour les codes des
utilisateurs non privilégiés par exemple.

La segmentation mémoire est largement obsolète et ne fonctionne pas en mode 64
bits, donc nous n'allons pas l'utiliser. Par contre, l'IDT en a besoin pour
accéder aux ISR pour des raisons de rétrocompatibilité.

### Driver PIC 8259A

Une fois que l'IDT fonctionne, il va falloir programmer le contrôleur
d'interruptions pour qu'il envoie le bon numéro d'interruption au CPU de manière
à ce que le bon ISR soit appelé.

Dans un chipset x86, toutes les interruptions hardware passent par le PIC
(Programmable Interrupt Controller) avant de passer par le CPU. Le PIC s'occupe
de sérialiser les interruptions hardware.

Par défaut, le PIC est configuré par le BIOS pour un PC IBM.

![](irq.png)

Problème, problème. Intel réserve les 31 premières interruptions pour les
exceptions logicielles telles que la division par 0. Donc, il faut remapper les
interruptions matérielles dans la plage 31-47.

Il faut lire le manuel du contrôleur et le configurer via ses ports d'E/S.

Une fois le PIC reprogramme il faut ajouter un mechanisme pour gerer les
interruptions hardware a l'IDT

---

![](idt-3.png)

---

### Driver PIT 8254

Le PIT (Programmable Interrupt Timer) 8254 est un contrôleur pour 3 oscillateurs
qui servent de timer pour le système. Le timer 3 est directement connecté au
PC-SPEAKER, et permet de jouer des fréquences normalement impossibles grâce à
une technique appelée "pulse width modulation".

Voici une explication provenant du wiki osdev:

> Le haut-parleur lui-même a deux positions possibles, "in" et "out". Cette
> position peut être définie via le bit 1 du port 0x61 sur le contrôleur de
> clavier. Si ce bit est défini (=1), le haut-parleur se déplacera vers la
> position "out", s'il est effacé (=0), alors le haut-parleur se déplacera vers la
> position "in". En se déplaçant à l'intérieur et à l'extérieur de manière
> répétée, des tons audibles sont produits si la vitesse de répétition (la
> fréquence) est dans la plage que le haut-parleur peut reproduire et que
> l'oreille humaine peut entendre. De plus, un seul mouvement vers l'intérieur ou
> vers l'extérieur produit un son de clic car il est si rapide. Ainsi, une
> fréquence trop basse pour être entendue comme un ton peut être perçue comme un
> bruit secoué ou bourdonnant. (En fait, toute fréquence produite par ce système
> produit également des fréquences plus élevées; recherchez "harmoniques de l'onde
> carrée" si cela vous intéresse.)
>
> En l'absence d'une véritable carte son, le haut-parleur du PC peut être utilisé
> pour produire un son numérique de qualité médiocre. Comme mentionné
> précédemment, le haut-parleur lui-même n'a que deux positions possibles, "en" et
> "hors". Cependant, plus de positions sont nécessaires pour jouer un son
> numérique. En général, 256 positions (8 bits) sont considérées comme adéquates
> pour produire un audio compréhensible. Une méthode populaire utilisée par de
> nombreux jeux vidéo sur PC pour contourner cette limitation s'appelle la
> modulation de largeur d'impulsion. Cette technique utilise les propriétés
> physiques du haut-parleur pour lui permettre de produire des sons (relativement)
> complexes qui existent dans l'audio 8 bits.
>
> Le haut-parleur du PC prend environ 60 millionièmes de secondes pour changer de
> position. Cela signifie que si la position du haut-parleur est changée de
> "dedans" à "dehors" puis changée de nouveau en moins de 60 microsecondes, le
> haut-parleur n'a pas eu suffisamment de temps pour atteindre complètement la
> position "dehors". En ajustant précisément la durée pendant laquelle le
> haut-parleur reste "dehors", la position du haut-parleur peut être réglée
> n'importe où entre "dedans" et "dehors", permettant au haut-parleur de former
> des sons plus complexes.

C'est donc grâce à l'oscillateur que l'on va exploiter cette technique et
produire les fréquences que l'on veut.

### Driver RTC

Le RTC (Real Time Clock) est un contrôleur qui donne accès au CMOS. Un chip
mémoire alimenté par une batterie pour persister les données entre les reboots.
Le CMOS contient plein de registres utilisés par le BIOS mais les seuls que l'on
peut utiliser sans risquer de casser l'ordinateur sont les registres de date et
heure. Le RTC contient également un oscillateur comme le PIT qui génère des
interruptions. Le contrôleur s'en sert pour mettre l'heure à jour. Nous nous en
servons pour mettre à jour l'affichage de l'heure.

# Conclusion

L'OS est composé de differentes parties qui peuvent être considérées comme
domaines de programmation par elles-mêmes, comme par example le système de
fichiers, le développement graphique, le kernel ou même la gestion des
procéssus.

Nous, nous avons choisi de faire des vérsions basiques des parties que nous
avons implementés pour respecter le sujet, mais cela à crée une difficulté
puisqu'on devait changer souvent de "thème" au milieu du projet.

D'autres difficultés rencontrées étaient:

- Le manque de documentation et ressources sur le développement d'un OS (par rapport à d'autres domaines de la programmation).
- le déboggage difficile, comme par example pour le driver _PS2_ et l'affichage _VBE_.
- l'organisation du temps par rapport aux tâches (certaines tâches s'averent plus difficile qu'on ne le pensait)

Mais ces difficultés nous ont permit d'évoluer des compétences liées à l'informatique, comme:

- Savoir lire la documentation officielle et s'aider des ressources sur internet.
- Comprendre le bas niveau et le fonctionnement de la machine.
- pouvoir écrire de l'_assembleur_ et comprendre son lien avec le langage _C_.
- débogger sans avoir accès à un déboggeur moderne et en partie sans affichage.

Aussi des compétences liées au travail:

- Savoir s'organiser
- Apprendre à travailler en équipe et se partager les tâches
- Gérer le temps réparti et faire le mieux de celui-ci
