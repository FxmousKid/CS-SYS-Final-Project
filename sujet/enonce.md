# Projet : planification de tâches

**L3 Informatique - Système**

Il est important de bien lire le sujet jusqu'au bout et de bien y
réfléchir avant de se lancer dans la programmation du projet.

## Sujet : `erraid` et `tadmor`, un outil de planification

Le but du projet est de programmer un couple démon-client permettant à
**un** utilisateur d'automatiser l'exécution périodique de tâches à des
moments spécifiés, à la manière de l'outil `cron`. L'utilisateur pourra
définir ou supprimer des tâches, consulter la liste des tâches définies,
obtenir des informations sur les exécutions passées d'une tâche...
L'outil devra pouvoir gérer des tâches correspondant à des commandes 
complexes (telles que des séquences ou des pipelines de commandes,
elles-mêmes simples ou complexes).

Plus précisément, `erraid` et `tadmor` doivent respecter les
prescriptions suivantes :


### Le démon

Le rôle du démon, `erraid`, consiste à exécuter les tâches définies
par le client. Pour cela, il doit _indéfiniment_ :
  - attendre et satisfaire les requêtes du client, `tadmor`,
  - exécuter les tâches aux dates demandées.

Une fois lancé (en tâche de fond), le démon `erraid` doit tourner jusqu'à
ce qu'une demande explicite de terminaison lui soit envoyée. Il doit dans
ce cas terminer le plus proprement possible.

Pour mener à bien son travail, le démon doit impérativement maintenir des
structures contenant toutes les informations nécessaires pour satisfaire 
les requêtes du client, qu'il s'agisse de la définition des tâches à
effectuer (à la fois la commande à exécuter et les prescriptions
horaires), ou du déroulement des tâches déjà effectuées. 

On demande par ailleurs que le démon puisse reprendre son travail après
une éventuelle interruption, (c'est-à-dire si un processus `erraid` a
terminé, et qu'un nouveau processus est lancé).  Il est donc nécessaire
que le démon stocke les informations nécessaires sur le disque. Celles-ci
seront regroupées dans une arborescence dont la racine pourra être passée
en paramètre à `erraid` grâce à une option `-r RUN_DIRECTORY` (avec comme
valeur par défaut `/tmp/$USER/erraid`).  Cette arborescence contiendra au
minimum une sous-arborescence `tasks` décrivant les tâches à effectuer,
selon le format décrit dans le fichier
[arborescence.md](arborescence.md).


### Le client

Le client `tadmor` a pour rôle de permettre à l'utilisateur d'envoyer des
requêtes au démon. Il reçoit en ligne de commande le descriptif de la
requête, qu'il formate puis transmet au démon. Il attend ensuite la
réponse du démon, qu'il interprète puis affiche le cas échéant sur sa
sortie standard dans un format adapté à un utilisateur humain avant de
terminer.

`tadmor` devra proposer au minimum les options suivantes :

##### Création/suppression de tâches

 * `-c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] CMD [ARG_1] ... [ARG_N]` : création d'une tâche simple 

 * `-s [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] TASKID_1 ... TASKID_N` :
   combinaison séquentielle des tâches d'identifiants `TASKID_1` à `TASKID_N`; la commande à exécuter est `(CMD_1 ; ... ; CMD_N)`

 * `-n` : en combinaison avec les précédentes, définition d'une tâche
   sans horaire d'exécution (donc qui ne sera jamais exécutée, et
   destinée à être combinée avec d'autres tâches pour créer une commande
   complexe)

 * `-r TASKID` : suppression de la tâche d'identifiant `TASKID`

##### Consultation des données du serveur

 * `-l` : liste des tâches

 * `-x TASKID` : liste datée des valeurs de retour des exécutions de la
   tâche d'identifiant `TASKID`

 * `-o TASKID` : sortie standard de la dernière exécution complète de la
   tâche d'identifiant `TASKID`

 * `-e TASKID` : erreur standard de la dernière exécution complète de la
   tâche d'identifiant `TASKID`


##### Divers

 * `-p PIPES_DIR` : définition du répertoire contenant les tubes de
   communication avec le démon (par défaut `/tmp/$USER/erraid/pipes`)

 * `-q` : arrêt du démon


### Communication

Pour communiquer, `erraid` et `tadmor` utilisent deux tubes nommés :
  - un _tube nommé de requête_ (dévolu à l'envoi des messages du client vers le démon, 
    appelés _requêtes_), de nom `erraid-request-pipe`,
  - un _tube nommé de réponse_ (dévolu à l'envoi des messages du démon vers le client, 
    appelés _réponses_), de nom `erraid-reply-pipe`.

Ces fichiers se trouvent dans un répertoire dédié, défini par l'option
`-p PIPES_DIR` du démon (par défaut le sous-répertoire `pipes` de son
arborescence de stockage).  Ils sont créés par le démon au démarrage
s'ils n'existent pas.

Les requêtes et les réponses doivent respecter le format décrit dans le
fichier [protocole.md](protocole.md), en utilisant les mêmes conventions
de [serialisation](sérialisation.md) que pour l'arborescence des tâches.


### Exemple d'utilisation :

```bash
$ tadmor -l                     # liste des tâches définies antérieurement
37: 30 19 1 echo "C'est lundi, c'est ravioli"
68: 0 5 * echo "Il est 5 heures, Paris s'éveille..."
$ tadmor -r 37                  # suppression des tâches antérieures
$ tadmor -r 68
$ tadmor -l                     # il n'y a plus de tâche définie
$ tadmor -c echo coucou         # création d'une tâche à exécuter à chaque minute
81
$ tadmor -c -m 0,3,6,9 date     # création d'une tâche avec horaires d'exécution précis
82
$ tadmor -c -n echo au revoir   # création d'une tâche "abstraite", sans exécution programmée
83
$ tadmor -l                     # liste des 3 tâches nouvellement créées
81: * * * echo coucou
82: 0,3,6 * * date
83: - - - echo au revoir
$ tadmor -x 81                  # un peu plus tard, quelques exécutions ont eu lieu
2025-10-21 17:59:00 0
2025-10-21 18:00:00 0
2025-10-21 18:01:00 0
2025-10-21 18:02:00 0
2025-10-21 18:03:00 0
2025-10-21 18:04:00 0
$ tadmor -x 82
2025-10-21 18:00:00 0
2025-10-21 18:03:00 0
$ tadmor -x 83                  # mais pas pour la tâche "abstraite"
$ tadmor -s 81 82 83            # création d'une nouvelle tâche par combinaison séquentielle des 3 préexistantes
84
$ tadmor -l                     # les sous-tâches combinées n'existent plus
84: * * * (echo coucou; date; echo au revoir)
$ tadmor -x 84                  # un peu plus tard...
2025-10-21 18:05:00 0
2025-10-21 18:06:00 0
2025-10-21 18:07:00 0
$ tadmor -o 84                  # sortie standard de la dernière exécution
coucou
mar. 21 oct. 2025 18:07:00 CEST
au revoir
```



## Modalités de réalisation

Le projet est à faire par équipe de 3 étudiants, exceptionnellement 2.
La composition de chaque équipe devra être envoyée par mail à
l'enseignante responsable du cours de systèmes avec pour sujet `[SY5]
équipe projet` **au plus tard le 7 novembre 2025**, avec copie à chaque
membre de l'équipe.

Chaque équipe doit créer un dépôt `git` **privé** sur le [gitlab de
l'UFR](https://moule.informatique.u-paris.fr/) **dès la constitution de
l'équipe** et y donner accès en tant que `Reporter` à tous les
enseignants du cours de Système : Isabelle Fagnot, Guillaume Geoffroy,
Pierre Letouzey, Anne Micheli, Astyax Nourel et Dominique Poulalhon.  Le
dépôt devra contenir un fichier `AUTHORS.md` donnant la liste des membres 
de l'équipe au format suivant :
```
- NOM Prénom - numéro étudiant - pseudo(s) apparaissant dans les commits
```

En plus du code source de vos programmes, vous devez fournir un
`Makefile` tel que :
  - l'exécution de `make` à la racine du dépôt crée (dans ce même
    répertoire) les exécutables `tadmor` et `erraid`,
  - `make distclean` efface tous les fichiers compilés,

ainsi qu'un mode d'emploi, et un fichier `ARCHITECTURE.md` expliquant la
stratégie adoptée pour répondre au sujet (notamment l'architecture
logicielle, les structures de données et les algorithmes implémentés).

**Le projet doit s'exécuter correctement sur lulu**.

Il est demandé de produire un couple démon-client fonctionnel dans le cas 
d'un unique utilisateur exécutant les deux programmes. Il
n'est pas demandé que le démon puisse gérer plusieurs requêtes
simultanées, ni s'adapter à un autre client potentiellement
non-coopératif (par exemple envoyant volontairement des requêtes
incomplètes).

Les seules interdictions strictes sont les suivantes : plagiat (d'un
autre projet ou d'une source extérieure à la licence, quelle qu'elle
soit), utilisation outrancière d'une IA générative de code, et
utilisation des fonctions de haut niveau masquant les appels système
étudiés en cours, par exemple la fonction `system` de `stdlib.h` ou
les fonctions  de `stdio.h` manipulant le type `FILE`.

Pour rappel, l'emprunt de code sans citer sa source est un
plagiat. L'emprunt de code en citant sa source est autorisé, mais bien
sûr vous n'êtes notés que sur ce que vous avez effectivement produit.
Donc si vous copiez l'intégralité de votre projet en le spécifiant
clairement, vous aurez quand même 0 (mais vous éviterez une demande de
convocation de la section disciplinaire).


## Modalités de rendu

Le projet sera évalué en 3 phases : deux jalons intermédiaires, sans
soutenance, et un rendu final avec soutenance. Les deux jalons
intermédiaires seront évalués par des tests automatiques.


### Premier jalon le 21 novembre

La version à évaluer devra être spécifiée à l'aide du tag `jalon-1` sur
la branche `main`.

Pour créer le tag : se placer sur le commit à étiqueter et exécuter :
```
git tag jalon-1
git push origin --tags
```

Le but de ce premier jalon est d'écrire la partie du démon qui exploite
une arborescence de tâches supposée préexistante et statique, incluant
uniquement des tâches simples ou combinées en séquences.

Points testés :

 - existence du dépôt git
 - présence du fichier `AUTHORS.md`
 - compilation sans erreur avec `make` à la racine du dépôt
 - interprétation d'une arborescence (fournie) définissant les tâches 
   à exécuter
 - exécution des tâches simples aux dates prescrites
 - exécution des séquences de tâches aux dates prescrites
 - mise à jour des fichiers de log (valeurs de retour et sorties standard)


### Deuxième jalon le 12 décembre

La version à évaluer devra être spécifiée à l'aide du tag `jalon-2`.

Le but de ce deuxième jalon est d'écrire les parties du démon et du
client permettant de gérer les requêtes *consultatives* : liste des
tâches, sortie et erreur standard de la dernière exécution complète d'une
tâche donnée, liste datée des valeurs de retour. 

Le démon devra donc à la fois exploiter une arborescence préexistante et
statique, et répondre aux requêtes du client.  On se limite toujours aux
tâches simples ou combinées en séquences. 


### Rendu final

La version à évaluer devra être spécifiée à l'aide du tag `rendu-final`.

Le projet final devra être rendu à une date encore à définir mais au plus
tard le 10 janvier, pour des soutenances dans la semaine du 12 janvier
2026.

### Participation effective

**Les projets sont des travaux de groupe, pas des travaux à répartir entre
vous** ("je fais le projet de Prog fonctionnelle, tu fais celui de
Systèmes"). La participation effective d'un étudiant au projet de son
groupe sera évaluée grâce, entre autres, aux statistiques du dépôt git et
aux réponses aux questions pendant la soutenance, et les notes des
étudiants d'un même groupe pourront être modulées en cas de participation
déséquilibrée. 

**En particulier, un étudiant n'ayant aucun commit sur les aspects
réellement "système" du code et incapable de répondre de manière
satisfaisante sera considéré défaillant.**

