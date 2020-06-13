<center>
<h1> Rapport de projet LO41 - UTBM </h1>
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
* NB_AVIONS *20* : nombre de processus avion à lancer  
* CAPACITE_PARKING *10* : capacité du parking de BSL (en nombre d'avions). Tant qu'il est plein, aucune clairance d'atterrissage ne pourra être donnée par le contrôleur.  
Si la ligne de commande ne contient pas exactement les deux paramètres ou qu'il ne s'agit pas d'entiers naturels, il signalera l'erreur mais démarrera tout de même avec les valeurs par défaut *ci dessus*.  

Pour lancer le programme avec les valeurs par défaut, il faudra donc utiliser la commande suivante :
```
./Simul_BSL 20 10
```

## Declaration de pannes et regeneration de l'ATIS ##
<u>Attention :</u> la déclaration des pannes ne peut se faire que pendant la navigation dans la région de contrôle avant l'atterrissage, c'est-à-dire pendant que l'avion affiche les informations de passage des points de report :
<font color="#8AE234">PH-EZSZ : Je passe le point N (sens : entree)</font>

<font color="#8AE234">PH-EZSZ : Je passe le point NE (sens : entree)</font>

<font color="#8AE234">PH-EZSZ : Je passe le point E (sens : entree)</font>

Pour déclarer une panne, il est nécessaire de démarrer le programme Simul_BSL d'abord (```./Simul_BSL <NB_AVIONS> <CAPACITE_PARKING>```), de répondre "O" à la question "Souhaitez-vous utiliser la console de controle ?" puis de démarrer la console de controle (```make run_interface```).  
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

## Jeux d'essai ##

### <u>Jeu d'essai 1 :</u> Capacité du parking non-atteinte (une seule piste en service) ###
<pre>Initialisation faite avec les valeurs par defaut : NB_AVIONS = 5 et CAPACITE_PARKING = 3 

Main PID : 15319
<font color="#75507B">*** ATIS : *******</font>
<font color="#75507B">Ici Bale-Mulhouse, information ALPHA enregistree a 14:47 UTC </font>
<font color="#75507B">    Piste en service legers et moyens porteurs : 15R</font>
<font color="#75507B">    Piste en service gros porteurs : 15L</font>
<font color="#75507B">    Vent du 37 DEG 29 KT</font>
<font color="#75507B">    Visibilite CAVOK</font>
<font color="#75507B">    Temperature 5 DEG CELSIUS</font>
<font color="#75507B">    Point de rosee 5 DEG CELSIUS</font>
<font color="#75507B">    Calage altimetrique QNH 1033 QFE 1002</font>
<font color="#75507B">    Informez ALPHA recue au premier contact</font>

*** MAIN : *******
Avion initialise | PID : 15324 | Immatriculation : PH-EZSZ

<font color="#8AE234">PH-EZSZ : Je passe le point N </font>

*** MAIN : *******
Avion initialise | PID : 15322 | Immatriculation : F-YGPJ

<font color="#8AE234">F-YGPJ : Je passe le point W </font>

<font color="#8AE234">PH-EZSZ : Je passe le point NE </font>

<font color="#8AE234">F-YGPJ : Je passe le point WA </font>

<font color="#8AE234">PH-EZSZ : Je passe le point E </font>

*** MAIN : *******
Avion initialise | PID : 15321 | Immatriculation : EC-JSYF

<font color="#8AE234">EC-JSYF : Je passe le point S </font>

<font color="#8AE234">F-YGPJ : Je passe le point WB </font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de PH-EZSZ bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Amsterdam - Schiphol </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 106 </font>
<font color="#34E2E2">            Passant le point E </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 15R</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">PH-EZSZ : Autorise atterrissage immediat piste 15R </font>

<font color="#34E2E2">PH-EZSZ : Autorise atterrissage piste  15R</font>

<font color="#8AE234">PH-EZSZ : Atterrissage effectue, piste 15R degagee, je roule pour le parking... Aurevoir </font>

<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">Places de parking occupees : 1/3</font>

<font color="#8AE234">EC-JSYF : Je passe le point E </font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de F-YGPJ bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Paris - Beauvais </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 109 </font>
<font color="#34E2E2">            Passant le point WB </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 15R</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">F-YGPJ : Autorise atterrissage immediat piste 15R </font>

<font color="#34E2E2">F-YGPJ : Autorise atterrissage piste  15R</font>

<font color="#8AE234">F-YGPJ : Atterrissage effectue, piste 15R degagee, je roule pour le parking... Aurevoir </font>

<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">Places de parking occupees : 2/3</font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de EC-JSYF bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Barcelone - El-Prat </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 109 </font>
<font color="#34E2E2">            Passant le point E </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 15R</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">EC-JSYF : Autorise atterrissage immediat piste 15R </font>

<font color="#34E2E2">EC-JSYF : Autorise atterrissage piste  15R</font>

<font color="#8AE234">EC-JSYF : Atterrissage effectue, piste 15R degagee, je roule pour le parking... Aurevoir </font>

<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">Places de parking occupees : 3/3</font>

*** MAIN : *******
Avion initialise | PID : 15325 | Immatriculation : G-YMXX

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             PID 15325</font>
<font color="#34E2E2">            Bale-Mulhouse de G-YMXX bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            A destination de Londres - Heathrow </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 126 </font>
<font color="#34E2E2">            Transpondeur 7000 </font>
<font color="#34E2E2">            Actuellement au parking </font>
<font color="#34E2E2">            Demandons roulage pour piste 15R</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">G-YMXX : Autorise decollage piste 15R, alignez-vous</font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">G-YMXX : Autorise decollage, je m&apos;aligne et decolle piste 15R</font>

<font color="#8AE234">*** AVION : *******</font>
<font color="#8AE234">G-YMXX : Decollage effectue, piste 15R liberee</font>

*** MAIN : *******
Places de parking occupees : 2/3

<font color="#8AE234">G-YMXX : Je passe le point E </font>

*** MAIN : *******
Avion initialise | PID : 15323 | Immatriculation : F-PRCJ

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             PID 15323</font>
<font color="#34E2E2">            Bale-Mulhouse de F-PRCJ bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            A destination de Bale-Mulhouse </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 117 </font>
<font color="#34E2E2">            Transpondeur 7000 </font>
<font color="#34E2E2">            Actuellement au parking </font>
<font color="#34E2E2">            Demandons roulage pour piste 15R</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">F-PRCJ : Autorise decollage piste 15R, alignez-vous</font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">F-PRCJ : Autorise decollage, je m&apos;aligne et decolle piste 15R</font>

<font color="#8AE234">*** AVION : *******</font>
<font color="#8AE234">F-PRCJ : Decollage effectue, piste 15R liberee</font>

<font color="#8AE234">G-YMXX : Je passe le point NE </font>

*** MAIN : *******
Places de parking occupees : 1/3

<font color="#8AE234">F-PRCJ : Je passe le point E </font>

<font color="#8AE234">G-YMXX : Je passe le point N </font>

<font color="#8AE234">F-PRCJ : Je passe le point NE </font>

<font color="#8AE234">F-PRCJ : Je passe le point N </font>
</pre>

### <u>Jeu d'essai 2 :</u> Capacité du parking atteinte (une seule piste en service) ###

<pre>Initialisation faite avec les valeurs par defaut : NB_AVIONS = 5 et CAPACITE_PARKING = 3 

Main PID : 15530
<font color="#75507B">*** ATIS : *******</font>
<font color="#75507B">Ici Bale-Mulhouse, information ALPHA enregistree a 14:50 UTC </font>
<font color="#75507B">    Piste en service legers et moyens porteurs : 33L</font>
<font color="#75507B">    Piste en service gros porteurs : 33R</font>
<font color="#75507B">    Vent du 208 DEG 3 KT</font>
<font color="#75507B">    Visibilite CAVOK</font>
<font color="#75507B">    Temperature 12 DEG CELSIUS</font>
<font color="#75507B">    Point de rosee 3 DEG CELSIUS</font>
<font color="#75507B">    Calage altimetrique QNH 950 QFE 919</font>
<font color="#75507B">    Informez ALPHA recue au premier contact</font>

*** MAIN : *******
Avion initialise | PID : 15534 | Immatriculation : G-JEHW

<font color="#8AE234">G-JEHW : Je passe le point N </font>

*** MAIN : *******
Avion initialise | PID : 15533 | Immatriculation : D-SIDT

<font color="#8AE234">D-SIDT : Je passe le point N </font>

<font color="#8AE234">G-JEHW : Je passe le point NE </font>

*** MAIN : *******
Avion initialise | PID : 15532 | Immatriculation : RA-DQKR

<font color="#8AE234">RA-DQKR : Je passe le point N </font>

<font color="#8AE234">D-SIDT : Je passe le point NE </font>

<font color="#8AE234">G-JEHW : Je passe le point E </font>

*** MAIN : *******
Avion initialise | PID : 15531 | Immatriculation : TC-QXKE

<font color="#8AE234">TC-QXKE : Je passe le point S </font>

<font color="#8AE234">RA-DQKR : Je passe le point NE </font>

<font color="#8AE234">D-SIDT : Je passe le point E </font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de G-JEHW bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Londres - Gatwick </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 131 </font>
<font color="#34E2E2">            Passant le point E </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 33L</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">G-JEHW : Autorise atterrissage immediat piste 33L </font>

<font color="#34E2E2">G-JEHW : Autorise atterrissage piste  33L</font>

<font color="#8AE234">G-JEHW : Atterrissage effectue, piste 33L degagee, je roule pour le parking... Aurevoir </font>

<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">Places de parking occupees : 1/3</font>

<font color="#8AE234">TC-QXKE : Je passe le point E </font>

*** MAIN : *******
Avion initialise | PID : 15535 | Immatriculation : F-JDJS

<font color="#8AE234">F-JDJS : Je passe le point N </font>

<font color="#8AE234">RA-DQKR : Je passe le point E </font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de D-SIDT bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Francfort - Rhin </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 124 </font>
<font color="#34E2E2">            Passant le point E </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 33L</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">D-SIDT : Autorise atterrissage immediat piste 33L </font>

<font color="#34E2E2">D-SIDT : Autorise atterrissage piste  33L</font>

<font color="#8AE234">D-SIDT : Atterrissage effectue, piste 33L degagee, je roule pour le parking... Aurevoir </font>

<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">Places de parking occupees : 2/3</font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de TC-QXKE bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Istambul - Ataturk </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 111 </font>
<font color="#34E2E2">            Passant le point E </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 33L</font>
<font color="#34E2E2">            </font>
<font color="#8AE234">F-JDJS : Je passe le point NE </font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de RA-DQKR bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Moscou - Sheremetyevo </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 103 </font>
<font color="#34E2E2">            Passant le point E </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 33L</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">RA-DQKR : Parking plein, executez deroutement ou mettez-vous en attente</font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">RA-DQKR : Parking plein, j&apos;attends...</font>

<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">TC-QXKE : Autorise atterrissage immediat piste 33L </font>

<font color="#34E2E2">TC-QXKE : Autorise atterrissage piste  33L</font>

<font color="#8AE234">TC-QXKE : Atterrissage effectue, piste 33L degagee, je roule pour le parking... Aurevoir </font>

<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">Places de parking occupees : 3/3</font>

<font color="#8AE234">F-JDJS : Je passe le point E </font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de F-JDJS bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Bale-Mulhouse </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 121 </font>
<font color="#34E2E2">            Passant le point E </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 33L</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">F-JDJS : Parking plein, executez deroutement ou mettez-vous en attente</font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">F-JDJS : Parking plein, j&apos;attends...</font>

</pre>

### <u>Jeu d'essai 3 :</u> déblocage du parking par un avion qui décolle (deux pistes en service) ###  
Ici, un phénomène est intéressant à observer : on observe qu'un premier avion déjà initialisé **F-PSVE** atterit normalement, et est le dernier à pouvoir rejoindre le parking qui est ensuite saturé. Ceci empêche un second avion **PH-QVXY** d'atterrir. Mais juste après, un avion s'initialise avec l'intention de décoller : **F-WBAP**. Celui est donc autorisé à sortir du parking et décoller. **PH-QVXY** est donc recontacté et finalement autorisé à atterrir car une place de parking peut maintenant l'accueillir.  

<pre><font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">F-PSVE : Autorise atterrissage immediat piste 15R </font>

<font color="#34E2E2">F-PSVE : Autorise atterrissage piste 15R</font>

<font color="#8AE234">F-PSVE : Atterrissage effectue, piste 15R degagee, je roule pour le parking... Aurevoir </font>

*** MAIN : *******
Places de parking occupees : 3/3

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             Bale-Mulhouse de PH-QVXY bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            En provenance de Amsterdam - Schiphol </font>
<font color="#34E2E2">            A destination de vos installations </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 134 </font>
<font color="#34E2E2">            Passant le point E </font>
<font color="#34E2E2">            Demandons integration pour atterrissage complet sur piste 15R</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">PH-QVXY : Parking plein, executez deroutement ou mettez-vous en attente</font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">PH-QVXY : Parking plein, j&apos;attends...</font>

*** MAIN : *******
Avion initialise | PID : 16646 | Immatriculation : F-WBAP

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">             PID 16646</font>
<font color="#34E2E2">            Bale-Mulhouse de F-WBAP bonjour </font>
<font color="#34E2E2">            Avion LEGER </font>
<font color="#34E2E2">            A destination de Bale-Mulhouse </font>
<font color="#34E2E2">            Avec l&apos;information ALPHA </font>
<font color="#34E2E2">            Vitesse de croisiere 100 </font>
<font color="#34E2E2">            Transpondeur 7000 </font>
<font color="#34E2E2">            Actuellement au parking </font>
<font color="#34E2E2">            Demandons roulage pour piste 15R</font>
<font color="#34E2E2">            </font>
<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">F-WBAP : Autorise decollage piste 15R, alignez-vous</font>

<font color="#34E2E2">*** AVION : *******</font>
<font color="#34E2E2">F-WBAP : Autorise decollage, je m&apos;aligne et decolle piste 15R</font>

<font color="#8AE234">*** AVION : *******</font>
<font color="#8AE234">F-WBAP : Decollage effectue, piste 15R liberee</font>

*** MAIN : *******
Places de parking occupees : 2/3

<font color="#8AE234">F-WBAP : Je passe le point E </font>

<font color="#CC0000">*** TWR : *******</font>
<font color="#CC0000">PH-QVXY : Autorise atterrissage immediat piste 15R </font>

<font color="#34E2E2">PH-QVXY : Autorise atterrissage piste 15R</font>

<font color="#8AE234">PH-QVXY : Atterrissage effectue, piste 15R degagee, je roule pour le parking... Aurevoir </font>
</pre>

### <u>Jeu d'essai 4 :</u> Simulation d'une panne ###

<!--TODO: inserer jeu d'essai -->