---
title: Rapport de stage OS 2024
author: Daniel Cojucari, Leonard Gomez
output: pdf_document
---

# Firmware carte mere

Certaines des sections suivantes parleront de concepts dans le contexte d'un système
qui utilise un firmware BIOS (Basic-Input-Output-System) et non pas un firmware
UEFI, qui est plus récent et a pour but de remplacer le BIOS.

Le BIOS est un firmware pour les cartes mère. Le mot _firm**ware**_ vient de
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
censés être interprètes au sens figuré. Comme dit Jean-Jacques Rousseau :

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

"Firm" veut aussi dire "entreprise", donc _firmware_ peut aussi être traduit
comme "outil venant d'entreprise" ce qui est une très jolie coïncidence.

![ROM BIOS chip](./bios_chip.jpg)


# Booting
- POST
- mbr (numero magic)
-
- russian dolls png
- stage 1
- stage 2

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
