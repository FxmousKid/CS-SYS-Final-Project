# Architecture des Fichiers

## Structure du repo

Le repo comporte le code source de **erraid** et de **tadmor**, ainsi que
les fichiers suivant :

- README.md : Des informations de base pour le projet
- AUTHOR.md : Les developeurs du projets
- code/ : Le code source
- docs/ : La documentation de l'architecture, potentiellement une config Doxygen

## Fichiers source

Les choix d'architecture suivant s'appliquent pour le code **d'erraid** et de **tadmor**

Les fichiers source (`.c` et `.h`) vivent `src/` et `include/`, respectivement.

On a opté pour que les 2 arborescences soient symmetriques, donc tout fichier
`src/.../f.c` aient sa contrepartie `include/.../f.h` avec `...` étant le
meme chemin.

Donc : 

```bash
$ tree src/

src
├── a
│   └── b
│       └── c
│           └── f1.c
└── d
    └── f2.c
```

```bash
$ tree include/

include/
├── a
│   └── b
│       └── c
│           └── f1.c
└── d
    └── f2.c
```

## Parsing

Pour le parsing, on implemente un parseur en descente recursive. On base notre traversée sur des axiomes
donnés dans le sujet, notammenent le faite que tout les noeuds non-feuilles sont un de : PL, SQ, IF, et les
feuilles sont des SI. Finalement dans la mémoire, on stock une sorte d'AST avec pas mal d'informations calculées
au moment du parsing afin de facilité l'execution le plus possible. Notamment a travers un ensemble de regles,
on determine au parsing si une commande output dans un fichier ou pas (sinon peut etre dans rien : condition d'un if, 
ou meme dans un pipe). On utilise un Tagged Union pour representer les differentes commandes.

## Exection

Pour l'execution, on se charge de verifier chaque minutes si il y a une tache a execute, si oui on fork un process
dedié a sa, et on revient immediatement verifier si le client a laissé un message dans le tube, a travers un appel
a poll(), et on se charge de traiter celle-ci, et on continue notre boucle.

### Nota Bene

Note sur l'apparence de code/erraid/src/main.c : c'est le résultat d'une experimentation sympa avec les tri/digraphs

## Feedback

C'etait un projet trés interessant, On a appris plein de choses, par contre ya 2-3 petits feedbacks :

au niveau du parsing de tadmor, le manque de " complique enormement le parsing : 
`-i 1 2 3`  est énervant a parser alors que adopter la meme approche que pour les -m aurait
simplifier et uniformiser le projet : `-i 1,2,3`
`-c -n echo -n 1 2 3` est encore plus enervant a parser, car on peut uniquement se baser sur l'ordre pour ne pas parser
tout ce qui est apres l'echo comme option a tadmor, le pire dans tout sa, c'est que cette approche est incompatible avec
getopt() / getopt_long() (ce qui est peut l'intention), mais sa rend la partie parsing d'autant plus lourde alors que ce projet
est presenté comme un projet non parsing intensif. ideallement, on a `-c "echo -n 1 2 3"` et apres nous meme
on peut faire un split_on_spaces() sur l'argument de -c pour utilise tout sa.

a par sa, c'est un projet vraiment sympa. (bien plus interessant qu'un shell, d'un point de vue connaissances, je trouve
que le shell beneficie plus, mais ce projet là necessite d'implementer unpeu de logique d'exec de shell aussi, donc
niveau shell tout n'est pas perdu, mais on travaille aussi avec de la serialization, du IPC, ...).

Donc finalement projet tres tres reussi, chapeau !
