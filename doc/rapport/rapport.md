---
title: Rapport de stage OS 2024
author: Daniel Cojucari, Leonard Gomez
output: pdf_document
---

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

Pour lire a partir d'un disque dans en "real mode" il faut utiliser l'API
fournie par le BIOS. Cette API est une table de fonctions, plus precisement
elle s'appelle l'IVT (Interrupt vector table). C'est une structure de donnees
propre au "real mode".

Chaque entree fais 4 octets:

+-----------+------------+------+
|  Segment  |  Offset    | Data |
+===========+============+======+
|32         |16          | Bits |
+-----------+------------+------+



## Multiboot

# Programmer un processeur x86
## Mode d'operation cpu
- Real mode
    Ce mode est le mode par defaut de CPU quand le systeme s'allume ou se reset. Il est cense simuler
    l'execution d'un CPU 8086. Dans ce mode nous avons access a toutes les interruptions
- protected mode
- long mode
## Gestion de la memoire
- Rien
- Segmentation
- Paging
### Mode d'operation vs gestion de la memoire

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
