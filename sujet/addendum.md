# Projet : planification de tâches

**L3 Informatique - Système**

## Liste des ajouts ou modifications effectués sur le sujet par rapport à sa version initiale


### Dans [protocole.md](protocole.md)

- `commandline` -> `command` et `COMMANDLINE` -> `COMMAND` 
  dans la description des réponses aux requêtes COMBINE

- `COMMAND` -> `ARGUMENTS` dans la description des requêtes CREATE et
  l'exemple final


### Dans [enonce.md](enonce.md)

#### Modification de la sémantique des options

##### Passage en majuscule des options de configuration d'`erraid` et `tadmor` 
- `-r RUN_DIR` -> `-R RUN_DIR`
- `-p PIPES_DIR` -> `-P PIPES_DIR`

##### Ajout d'une option à `erraid`

- `-F` pour forcer l'exécution en avant-plan (sans démonisation)

##### Ajout d'options à `tadmor` 

- `-i` pour la combinaison conditionnelle
- `-p` pour la combinaison en pipeline

#### Précision sur l'affichage attendu pour l'option `-l`

L'affichage comprend une ligne par tâche, dans un ordre quelconque, avec
pour chaque tâche, séparés par des espaces :

 * son identifiant suivi de `:`
 * ses dates d'exécution programmées au format usuel de `cron`, ou `- - -` pour les tâches abstraites
 * la commande à exécuter, au format suivant :
    - chaque sous-combinaison de commandes est délimitée par des parenthèses,
    - les commandes simples ne sont pas parenthésées,
    - les parenthèses externes autour d'une combinaison de commandes sont facultatives,
    - les commandes (simples ou combinées) combinées en séquence sont séparées par des `;`,
    - les commandes (simples ou combinées) combinées en pipeline sont séparées par des `|`,
    - les sous-commandes d'une combinaison conditionnelle sont délimitées par les mots-clés `if`, `then`, (`else`) et `fi`,
    - entre une sous-commande simple d'une combinaison conditionnelle et le mot-clé suivant, le délimiteur `;`
      est impératif,
    - il n'y a pas de `;` entre une sous-commande d'une combinaison conditionnelle elle-même encadrée de parenthèses et le mot-clé suivant,
    - les espaces autour des parenthèses et des connecteurs `;` et `|`
      sont facultatifs.


#### Exemple d'exécution enrichi

Ajout d'une combinaison de commandes par pipeline et d'une combinaison conditionnelle


#### Rendu final

Pour permettre le regroupement correct des commits de chaque auteur par
`git shortlog`, ajouter un fichier `.mailmap` contenant une ligne de la forme
```
NOM Prénom <adresse-mail>
```
pour chaque adresse mail apparaissant dans les commits, par exemple :
```
LAMPION Séraphin <seraphin.lampion@etu.u-paris.fr>
LAMPION Séraphin <seraphin2735@assurances-mondass.com>
```

Le projet final devra être rendu ~~à une date encore à définir mais au
plus tard le 10~~ le 11 janvier au soir, pour des soutenances ~~dans la
semaine du 12~~ les 13, 14 et 15 janvier 2026.

