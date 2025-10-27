## Arborescence des tâches

L'arborescence de stockage utilisée par le démon contient entre autres
une sous-arborescence `tasks` décrivant l'ensemble de tâches à effectuer.
Plus précisément, le démon attribue à chaque tâche à effectuer un
identifiant unique, qui sert de nom à un répertoire contenant tous les
fichiers (ordinaires et sous-répertoires) qui la concernent, en
respectant le format de sérialisation décrit dans le fichier
[serialisation.md](serialisation.md)) :

 - un sous-répertoire `cmd` décrivant la commande à exécuter,
 - un fichier `timing` décrivant les dates et horaires auxquels la tâche doit être
   exécutée,
 - un fichier `times-exitcodes` contenant la liste datée des valeurs de
   retour des exécutions précédentes,
 - un fichier `stdout` contenant la sortie standard de la dernière
   exécution complète,
 - un fichier `stderr` contenant la sortie erreur standard de la dernière
   exécution complète.

Le sous-répertoire `cmd` contient lui-même :
 - un fichier `type` (de type `uint16`) indiquant s'il s'agit d'une commande 
   simple ou complexe,
 - s'il s'agit d'une commande simple, un fichier `argv` au format `arguments`,
 - sinon, un sous-répertoire par sous-commande.


#### Exemples

Le squelette d'arborescence ci-dessous correspond à deux tâches, portant
respectivement les identifiants `412` et `923`; `412` est une tâche
simple et `923` une combinaison de trois sous-tâches, dont deux sont
simples et l'autre est elle-même combinaison de deux sous-tâches simples;
la tâche `412` a été exécutée au moins une fois (d'où l'existence des
fichiers `stdout` et `stderr`), contrairement à la tâche `923`.

```sh
tasks/
├── 412
│   ├── cmd
│   │   ├── argv
│   │   └── type
│   ├── stderr
│   ├── stdout
│   ├── times-exitcodes
│   └── timing
└── 923
    ├── cmd
    │   ├── 0
    │   │   ├── argv
    │   │   └── type
    │   ├── 1
    │   │   ├── 0
    │   │   │   ├── argv
    │   │   │   └── type
    │   │   ├── 1
    │   │   │   ├── argv
    │   │   │   └── type
    │   │   └── type
    │   ├── 2
    │   │   ├── argv
    │   │   └── type
    │   └── type
    ├── times-exitcodes
    └── timing
```


Plusieurs archives se trouvent dans le répertoire [exemples-arborescences](exemples-arborescences).
Désarchivez-les et examinez soigneusement le contenu des différents
fichiers (avec `od` ou `hexdump` au besoin) :

- la première définit deux tâches simples, qui n'ont pas encore été
  exécutées;

- la deuxième définit une séquence de 4 tâches simples, qui a déjà été
  exécutée deux fois;

- la troisième définit une séquence de 3 séquences (de 4 tâches simples), 
  qui a déjà été lancée une fois;

- la quatrième archive est destinée à mieux comprendre le processus de
  création des tâches complexes : elle reflète l'état de la troisième
  arborescence juste avant la combinaison des 3 séquences. À ce stade,
  trois tâches "abstraites" ont été définies, avec chacune une commande
  complexe à exécuter, mais sans date d'exécution. La combinaison de ces
  trois tâches les "consomme" -- elles sont supprimées de la liste des
  tâches et toutes les informations associées sont effacées (identifiant,
  timing, trace des éventuelles exécutions passées) à l'exception de
  leurs arborescences `cmd` qui deviennent les sous-arborescences de
  l'arborescence `cmd` de la tâche nouvellement créée.
