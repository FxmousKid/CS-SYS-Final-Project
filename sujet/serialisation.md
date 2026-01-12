# Projet : planification de tâches

**L3 Informatique - Système**

## Format de sérialisation des données

La communication entre le client et le serveur, ainsi que le stockage
dans l'arborescence des tâches, doivent respecter les représentations
suivantes :


#### Les types entiers

`uint8`, `uint16`, `uint32`, `uint64` : entiers non signés sur 1, 2, 4 et
8 octets respectivement, en convention big-endian. 

`int8`, `int16`, `int32`, `int64` : entiers signés sur 1, 2, 4 et 8
octets respectivement, en convention big-endian, complément à 2.

_Remarque : le projet doit fonctionner sur `lulu`, sans exigence de
portabilité sur tous les systèmes POSIX; vous pouvez donc utiliser les
fonctions de conversion disponibles sous linux et listées dans `man 3
endian`._


#### Le type `string`

Composé d'un sous-champ `LENGTH` de type `uint32` contenant la longueur
de la chaîne (sans le 0 terminal), et d'un sous-champ `DATA` formé des 
`LENGTH` octets correspondant au contenu de la chaîne :

```
LENGTH=L <uint32>, DATA[0] <uint8>, ..., DATA[L-1] <uint8>
```

#### Le type `timing`

Décrit un ensemble de minutes (comme les 5 premiers champs d'une ligne de `crontab`). 
Composé d'une succession de 3 sous-champs :

```
MINUTES <uint64>, HOURS <uint32>, DAYSOFWEEK <uint8>
```

Chacun des sous-champs est à voir comme un tableau de bits indiquant si
la commande doit être exécutée ou non à la minute / l'heure / le jour
correspondants.

`MINUTES` : bit de poids faible (n°0) = minute 0, ..., bit de poids fort (n°59) = minute 59.
  
 - Exemple : 0x00002000000007F0 = de la minute 4 à la minute 10, puis à la minute 45
             (équivalent dans `crontab` : 4-10,45)

`HOURS` : bit n°0 = heure 0, ..., bit n°23 = heure 23

 - Exemple : 0x00041100 = à 8h, 12h et 18h
             (équivalent dans `crontab` : 8,12,18)

`DAYSOFWEEK` : bit n°0 = dimanche, ..., bit n°6 = samedi.
 
 - Exemple : 0x5C (en binaire : 01011100) = du mardi au jeudi, et le samedi
             (équivalent dans `crontab` : 2-4,6)


#### Le type `arguments`

```
ARGC <uint32>, ARGV[0] <string>, ..., ARGV[ARGC-1] <string>
```

`ARGC` doit être au moins égal à 1. `ARGV[0]` contient le nom de la commande à appeler et doit être non vide.


#### Le type `command`


Soit (pour une commande simple) : 
```
TYPE='SI' <uint16>, ARGS <arguments>
```

Soit (pour une combinaison de commandes) : 

```
TYPE <uint16>, NBCMDS=N <uint32>, CMD[0] <command>, ..., CMD[N-1] <command>
```

avec un `TYPE` autre que `'SI'`, par exemple `TYPE='SQ'` pour les
séquences, `TYPE='PL'` pour les pipelines, ou `TYPE='IF'` pour les
conditionnelles.

