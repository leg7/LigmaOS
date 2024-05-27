---
colorlinks: true
title: Rapport de stage OS 2024
author: Daniel Cojucari, Leonard Gomez
output: pdf_document
---

# Environement de development

Pour tester notre OS on pourrait flasher l'ISO sur une clef USB a chaque
recompilation et demarer un PC pour tester mais ce workflow ne permet pas
d'iterer rapidement. Le processus prendrait plusieures minutes dans le
meilleure des cas et il serait impossible de debugger l'OS avec un debugger.

Avec une machine virtuelle par contre une seule commande suffit pour recompiler
et redemarrer l'OS tres rapidement.
On a donc choisie d'utiliser QEMU pour simuler un PC avec un cpu x86 32 bits

TODO: terminer

# Programmer un processeur x86

Un OS fournit une abstraction des composants materielles et leurs specificitees.
L'un des ces composants a abstraire est le CPU. Un CPU fonctionne differement
selon son architecture et son design un CPU x86 ne fonctionne pas comme un
CPU RISC-V, qui lui ne fonctionne pas comme qu'un CPU Motorolla 68k.

Pour programmer cette abstraction correctement il faut comprendre son
fonctionnement.

Comment fonctionne un CPU x86 ?

## Mode d'operation

- **Real mode**

    > C'est le mode par defaut du CPU quand le systeme s'allume ou se reset.
    > Il simule l'execution d'un CPU 8086 16 bits. Son nom vient du fait que
    > dans ce mode toutes les addresse memoires correspondent aux vraies
    > addresse physiques, il n'y a aucune protection de la memoire.
    > Dans ce mode nous avons access a toutes les interruptions de l'IVT
    > configuree par le BIOS et 1MB de RAM.

- **Protected mode**

    > Ce mode passe le CPU en execution 32 bits. "Protected" parce que des
    > moyens de protection de la memoire sont mis en place : organization
    > des segments et restriction d'access par privilege avec la GDT,
    > memoire virtuelle et pagination.
    > Comme les registres font maintenant 32 bits et chaque processus
    > peut avoir 4GB de memoire virtuelle.
    > Pour activer ce mode il faut mettre le bit PE du registres CR0 a 1
    > initialiser la gdt et desactiver les interruptions temporairement.
    > ```asm
    >    cli            ; desactiver les interruptions
    >    lgdt [gdtr]    ; charger la GDT
    >    mov eax, cr0
    >    or al, 1       ; activer le bit PE
    >    mov cr0, eax
    >```

- **Long mode**

    > Ce mode ressemble beaucoup au mode protege mais certaines fonctionalitees
    > comme la segmentation sont completement desactives en faveur de la
    > pagination et la memoire virtuelle. De plus les registres generaux sont
    > etendus a 64 bits et 8 nouveaux registres d'entiers et de vecteurs sont
    > ajoutes (R8-R15, XMM8-XMM15). Les physiques addresses utilisent 40 bits,
    > et les addresses virtuelles 48. Pour passer en long mode il faut avoir
    > configure la memoire virtuelle au prealable.

Nous avons choisi d'utiliser le mode protege parce qu'il nous donne access au
plus de fonctionalites sans avoir a configurer des structures de donnees
complexes que l'on ne peut pas encore tester sans OS.
Cependant si on continue l'OS il serait judicieux de passer en long mode rapidement
et de se debarasser du code 32 bits parce que intel a annonce en en 2023 que
d'ici 2025 ils ne produiront plus de CPUs 32-bits et donc le mode protege
deviendra obsolete bientot apres.

Cette decision implique qu'il faudra utiliser un cross-compiler pour compiler du
code 32bits sur notre machine.

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

Au demarrage, le BIOS execute le POST (Power-on self-test). Cette routine vas
faire plusiers choses:

- Verifier l'état du CPU et ses registres
- Verifier l'integrité du BIOS
- Detecter tous les composants (RAM, controlleurs, GPU, chipset)
- Verifier s'ils fonctionnent
- Les initialiser
- **Initialise le CPU en "real mode"**
- **Detecter quelle disque contient l'OS**
- **Preparer l'API des composants**

Nous allons nous intereser a ces deux dernières étapes.

## MBR (Master boot record)

Tout disque partitioné correctement possede un section MBR au début du disque.
Le MBR est situé sur le premier secteur du disque et occupe 512 octets.

Pendant le POST le BIOS vas lire l'octet 510 du MBR et le comparer a la valeure
magique `0x55AA`. Si l'octet correspond au nombre magique le système comprend
que c'est sur ce disque que l'OS est se trouve.

Le BIOS vas ensuite nous donner le control en chargeant les 510 premiers octets
du disque a l'addresse memoire `0x7C00` pour ensuite executer les données a cette
addresse.

Ce programme est le "stage 1" du bootloader.

## Stage 1

Voici un exemple simple:

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

Ce programme est limité a une taille de 510 octets parce que sinon il ecrasera
le nombre magique. Il faut donc charger la suite de notre bootloader en memoire
et l'executer avant d'atteindre la limite.

## Stage 2

Pour charger le stage 2 de notre bootloader il faut pouvoir lire le programme
sur le disque et le charger en memoire.

Pour lire a partir d'un disque en "real mode" il faut utiliser l'API
fournie par le BIOS. Cette API est une table de fonctions, plus precisement
elle s'appelle l'IVT (Interrupt vector table). C'est une structure de donnees
propre au "real mode" qui est liée au CPU directement par le mechnisme
d'interruptions.

Chaque entree fais 4 octets:

+-----------+------------+---+
|  Offset   |  Segment   |   |
+===========+============+===+
|0          |16          | 32|
+-----------+------------+---+

> Segment un selecteur de segment memoire et offset le decalage dans ce segment

La table a 256 entrees et fait donc 1024 octets. Elle se situe a l'addresse 0x0,
oui l'addresse NULL est enfaite une addresse valide, celle de l'IVT en
l'occurrence.

Pour appeler une fonction dans cette table il faut simuler une interruption
avec l'instruction `int` mnemonic pour "interrupt". Par exemple `int 0x08` vas
simuler l'interruption 8 et le CPU vas alors chercher la fonction a l'offset 8
dans la table (index 7) et l'executer.\
En fonction des valeures dans le registres genereaux le fonctions vont changer
de comportement. Les registres sont alors les parametres des fonctions.

La reference pour savoir se servir des ces fonctions est le
[Ralph's brown interrupt list](https://www.cs.cmu.edu/~ralf/files.html).

Apres avoir essaye pendant quelques jours nous avons decides de ne pas faire notre
propre bootloader pour pouvoir aller plus loin dans le development de l'OS et
ne pas passer 4 semaines a ecrire un bootloader.

## Multiboot

Le standard multiboot permet a un kernel de communiquer avec un bootloader
multiboot par une structure de donnees standard.

TODO: terminer

## IO en x86

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
