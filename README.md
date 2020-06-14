<center>
<h1> README projet LO41 - UTBM </h1>
<h2> Simulation d'un aéroport </h2>
<h3> Valentin Mercy </h3></center>

## Introduction
Le projet proposé ici consiste à simuler le comportement d'avions de plusieurs catégories (légers, moyens porteurs et gros porteurs) évoluant autour d'un aéroport français contrôlé, à deux pistes, selon les règles de la circulation aérienne. Qu'ils soient au départ ou à l'arrivée de cet aéroport, il s'agit de veiller à ce que tous les aéronefs puissent, par le biais de communications avec la tour de contrôle de l'infrastructure aéroportuaire, bénéficier des services de contrôle et d'alerte leur permettant de partager les deux pistes bidirectionnelles à disposition selon leur gabarit, leur ordre de départ/arrivée et la direction du vent. Des cas de pannes doivent également être gérés : il s'agit là de promettre à un avion dans une situation de détresse (Mayday) d'être prioritaire quant à l'utilisation des pistes en service.  

## Compilation ##
Les règles mises en place dans le Makefile permettent une compilation facilitée via les commandes suivantes :  
* Compilation sans execution :
```
make build
```

* Compilation avec execution :
```
make all
```

* Compilation et exection de la console de controle (pour déclarer des pannes et regénérer l'ATIS) :
```
make interface
```

## Lancement du programme en ligne de commande ##
Le programme attend les paramètres suivants (dans l'ordre) à passer en argument sur la ligne de commande :
* NB_AVIONS *5* : nombre de processus avion à lancer  
* CAPACITE_PARKING *5* : capacité du parking de BSL (en nombre d'avions). Tant qu'il est plein, aucune clairance d'atterrissage ne pourra être donnée par le contrôleur.  
Si la ligne de commande ne contient pas exactement les deux paramètres ou qu'il ne s'agit pas d'entiers naturels, il signalera l'erreur mais démarrera tout de même avec les valeurs par défaut *ci dessus*.  

Pour lancer le programme avec les valeurs par défaut, il faudra donc utiliser la commande suivante :
```
./Simul_BSL 5 5
```

## Declaration de pannes et regeneration de l'ATIS ##
<u>Attention :</u> la déclaration des pannes ne peut se faire que pendant la navigation dans la région de contrôle avant l'atterrissage, c'est-à-dire pendant que l'avion affiche les informations de passage des points de report :
<pre>
<font color="#8AE234">PH-EZSZ : Je passe le point N (sens : entree)</font>

<font color="#8AE234">PH-EZSZ : Je passe le point NE (sens : entree)</font>

<font color="#8AE234">PH-EZSZ : Je passe le point E (sens : entree)</font>
</pre>
Pour déclarer une panne, il est nécessaire de démarrer le programme Simul_BSL d'abord (```./Simul_BSL <NB_AVIONS> <CAPACITE_PARKING>```), de répondre "O" à la question "Souhaitez-vous utiliser la console de controle pour declarer des pannes ou regenerer l'ATIS ?" puis de démarrer la console de controle (```make run_interface```).  
Reperez ensuite le PID de l'avion qui doit être la cible de la panne, puis saisissez-le dans l'interface de contrôle.


## Ordre d'affichage des informations de suivi ##
**Attention :** Lorsque le programme est exécuté, il affiche sur la sortie standard les différentes actions des avions et de la tour de contrôle. Ces affichages sont protégés par un Mutex pour ne pas être interrompus mutuellement. L'ordre d'affichage de ces actions ne correspond pas forcément à leur déroulé dans le temps (qui dépend d'un temps d'attente généré aléatoirement - l'attente se faisant juste après la création du processus) mais à l'ordonnancement des processus. Ainsi, il n'y a rien d'anormal à voir par exemple la tour écrire :
```
*** TWR : *******
RA-PHAS : Piste occupee, executez tour d'attente...
```
avant qu'elle n'écrive plus bas : 
```
*** TWR : *******
RA-QOEO : Autorise atterrissage immediat
```
Dans cet exemple, le processus correspondant à l'avion **RA-QOEO** a attendu moins longtemps que celui qui correspond à **RA-PHAS**, ce dernier est donc mis en attente. Mais l'ordre d'affichage de ces événements ne correspond pas à leur ordre de survenue.

## Troubleshooting ##
```
Makefile:18: recipe for target 'run' failed
make: *** [run] Segmentation fault (core dumped)
```
Après certaines manipulations dans le code, il se peut que vous tentiez de créer des sémaphores par dessus d'autres qui n'ont pas été détruits à la fin de l'exécution précedente, notamment le sémaphore **print** qui gère l'affichage sur la sortie standard.  
Dans ce cas, il suffit de commenter les lignes 11,12,52,53 de **format.c**, puis de lancer un ```make all``` avant de stopper l'exécution. Il ne reste plus qu'à décommenter les lignes précedemment commentées puis à relancer le programme normalement à nouveau avec ```make all```.