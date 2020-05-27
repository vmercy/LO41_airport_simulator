<center>
<h1> Rapport de projet LO41 - UTBM </h1>
<h2> Simulation d'un aéroport </h2>
<h3> Valentin Mercy </h3></center>

## Introduction <!-- omit in toc -->
Le projet proposé ce semestre consiste à simuler le comportement d'avions de plusieurs catégories (légers, moyens porteurs et gros porteurs) évoluant autour d'un aéroport français contrôlé, à deux pistes, selon les règles de la circulation aérienne. Qu'ils soient au départ ou à l'arrivée de cet aéroport, il s'agit de veiller à ce que tous les aéronefs puissent, par le biais de communications avec la tour de contrôle de l'infrastructure aéroportuaire, bénéficier des services de contrôle et d'alerte leur permettant de partager les deux pistes bidirectionnelles à disposition selon leur gabarit, leur ordre de départ/arrivée et la direction du vent. Des cas de pannes doivent également être gérés : il s'agit là de promettre à un avion dans une situation de détresse (Mayday) d'être prioritaire quant à l'utilisation des pistes en service.  
En tant qu'élève pilote privé, ce projet m'a beaucoup intéressé, car il s'intègre parfaitement dans ce contexte en tant qu'application indirecte des règles de base de la circulation aérienne qui font partie de mon apprentissage, moyennant les différentes hypothèses simplificatrices que j'ai du mettre en place. En effet, seule une petite partie des règles de circulation aérienne sera traitée dans ce projet, de par leur complexité et leur nombre en réalité très élevés. Toutefois, je me suis attaché à garder un lien fort avec la réalité dans mes choix de conception (présentés en intégralité dans la section [Choix de conception](#choix-de-conception)) :  
* l'aéroport que j'ai choisi est bien réel : il s'agit de l'aéroport internationnal le plus proche de Belfort, soit Bâle-Mulhouse (BSL). Les canaux radio et leurs fréquences associées (GND = sol, ATIS = information automatique, APP = approche, TWR = tour) sont également conformes à la réalité. Cet aéroport présente également l'avantage de comporter 2 pistes bidirectionnelles (soit 4 en tout), que j'ai approximées parallèles (j'ai donc du les suffixer d'un "R" et d'un "L" conformément à la norme internationale en vigueur).  
* les règles de base de la phraséologie aéronautique seront reproduites dans la mesure du possible, c'est-à-dire en veillant à ce que le résultat soit intelligible par des non-initiés : épellation OTAN des immatriculations et des points de report, immatriculations en fonction des pays d'origine, codes transpondeur, signal de détresse *Mayday Mayday Mayday*.  
* comme demandé dans le sujet, les instructions seront données par le contrôleur à chaque avion sous forme de clairances (= autorisations) : clairance d'approche, de décollage et d'atterrissage.  
* codes d'aéroports (IATA = International Air Transport Association et OACI = Organisation de l'Aviation Civile Internationale)
* noms des points de reports et itinéraires préférentiels de la [carte d'approche à vue](https://www.sia.aviation-civile.gouv.fr/dvd/eAIP_21_MAY_2020/Atlas-VAC/PDF_AIPparSSection/VAC/AD/AD-2.LFSB.pdf) :
<center>
<figure>
    <img src="images/vac_simplifiee.png">
    <figcaption><i><u><b>Figure 1 :</b></u> Points de report et itinéraires préférentiels publiés sur la carte VAC de l'aéroport de Bâle-Mulhouse</i></figcaption>
</figure>
</center>

## Table des matières <!-- omit in toc -->
- [Choix de conception](#choix-de-conception)
  - [Paramètres (constantes) et valeurs](#paramètres-constantes-et-valeurs)
  - [Lancement du programme en ligne de commande](#lancement-du-programme-en-ligne-de-commande)
  - [Hypothèses simplificatrices](#hypothèses-simplificatrices)
  - [Structures de données](#structures-de-données)
    - [Pays](#pays)
    - [Point de report](#point-de-report)
    - [Itinéraire](#itinéraire)
    - [Aéroport](#aéroport)
    - [Avion](#avion)
  - [Eléments générés aléatoirement](#eléments-générés-aléatoirement)
  - [Eléments communicant entre eux](#eléments-communicant-entre-eux)
  - [Signaler un avion en détresse](#signaler-un-avion-en-détresse)
- [Conclusion](#conclusion)
- [Sources](#sources)
- [Annexes](#annexes)
  - [<u>Annexe 1 :</u> Jeu d'essai avec 10 avions](#uannexe-1-u-jeu-dessai-avec-10-avions)

## Choix de conception ##
Cette section présente les différents choix de conception que j'ai portés au projet. Le but du projet étant d'utiliser au maximum et dans le meilleur contexte les différents concepts vus en cours (processus, signaux, objets IPC : files de messages, sémaphores, mémoire partagée), je préciserai dans cette section à quel moment ils interviennent. Pour ne pas surcharger le projet, je n'ai pas cherché à intégrer absolument tous les concepts vus en cours, seulement ceux qui se prêtent effectivement à la simulation demandée.
### Paramètres (constantes) et valeurs ###
```
LONGMAX_SUFFIXE_IMMAT 4
LONGMAX_PREFIXE_IMMAT 2 //longueur maximum du prefixe de l'immatriculation d'un avion (celles-ci dépendent des pays où sont immmatriculés les avions)
LONGMAX_PREFIXE_OACI 2 //longueur maximum du prefixe du code OACI d'un aéroport
LONGMAX_SUFFIXE_OACI 2 //longueur maximum du suffixe du code OACI d'un aéroport
LONGMAX_INDICATIF_PT_REPORT 2 //longueur maximum d'un indicatif de point de report
NB_PTS_REPORT 7 //nombre de points de report autour de BSL
LONGMAX_ITINERAIRE 3 //nombre maximum de points de report pour un itineraire
LONGMAX_NOM_AEROPORT 30 //longueur maximum du nom d'un aeroport 
LONGMAX_IATA 3 //longueur max d'un code IATA
```
### Lancement du programme en ligne de commande ###
Le programme attend les paramètres suivants (dans l'ordre) à passer en argument sur la ligne de commande :
* NB_AVIONS *20* : nombre de processus avion à lancer  
* CAPACITE_PARKING *10* : capacité du parking de BSL (en nombre d'avions). Tant qu'il est plein, aucune clairance d'atterrissage ne pourra être donnée par le contrôleur.  
Si la ligne de commande ne contient pas exactement les deux paramètres ou qu'il ne s'agit pas d'entiers naturels, il signalera l'erreur mais démarrera tout de même avec les valeurs par défaut *ci dessus*.  

Pour lancer le programme avec les valeurs par défaut, il faudra donc utiliser la commande suivante :
```
./Simul_BSL 20 10
```

### Hypothèses simplificatrices ###
Comme mentionné dans l'introduction, plusieurs hypothèses simplficatrices ont été mises en place pour ne pas dépasser le sujet :
* La zone géographique étudiée s'arrêtera à la CTR (Controlled Traffic Region) de BSL. Toutefois, les points de report qui y sont extérieurs seront compris dans l'étude. Ainsi, un avion pourra être déclaré en panne n'importe où sur son itinéraire.  
* Un avion sera considéré géographiquement dans l'un des états suivants :
  * Au sol (phase de roulage avant décollage ou après atterrissage). Le sol (aire de manoeuvre) contient un parking de capacité paramétrée  
  * En vol : dans ce cas, sa position est identifiée par le dernier point de report franchi.  
* Les communications se feront en français quel que soit le pays de provenance/destination de l'avion (en réalité, ces communications se font pour la majorité en anglais)  
* L'aéroport BSL étant en réalité composé de deux terminaux (un côté suisse, un côté français) nous considérerons uniquement le terminal français.
* Tous les avions porteront une immatriculation portant le préfixe OACI du pays étranger de provenance ou (exclusif) de destination. Si ces deux pays sont tous les deux la France (cas d'un vol intérieur), l'avion portera une immatriculation française (F-XXXX).  
* Lorsqu'un avion se trouve dans la zone géographique de l'étude, il ne peut dévier de son itinéraire initial qu'en cas de panne (pas de déroutement vers un autre aéroport).  
* Les pistes, qui sont en réalité sécantes, seront ici considérées parallèles (voir figure 2). Cela nous permettra de faire décoller et atterrir deux avions simultanément.  
  * une piste bidirectionnelle pour les avions légers et les moyens porteurs : la 33L/15R (vent du nord/vent du sud), de longueur 4000m (longueur de la piste réelle 33 de BSL), réservée aux gros porteurs  
  * une piste bidirectionnelle pour les gros porteurs : la 33R/15L (vent du nord/vent du sud), de longueur 2500m (longueur fictive imposée), réservée aux avions légers et moyens porteurs  
<center>
<figure>
    <img src="images/pistes_simplifiees.png">
    <figcaption><i><u><b>Figure 2 :</b></u> Représentation des pistes fictives utilisées dans ce projet</i></figcaption>
</figure>
</center>
  
### Structures de données ###
Plusieurs structures de données ont été mises en place pour traiter le problème dans de bonnes conditions.
#### Pays ####
Chaque pays européen hébergeant un aéroport considéré dans ce projet est représenté par la structure suivante :
```
struct pays{
  char prefixe_immat[LONGMAX_PREFIXE_IMMAT+1];  //prefixe de l'immatriculation d'un avion en provenance/a destination du pays
  char prefixe_aeroport[LONGMAX_PREFIXE_OACI+1]; //prefixe du code OACI d'un aeroport heberge par le pays
}
```

#### Point de report ####
Un point de report est un point autour de l'aéroport, qui permet aux avions de signaler leur position (radial à suivre et temps estimé pour arriver à ce point).
Les environs de Bâle-Mulhouse contiennent les points de report suivant : N (pour "Nord", se lit "Novembre"), NE ("Novembre Echo" pour "Nord Est"), E, S, WB, WA, W. Chaque point de report est associé à une altitude préférentielle (exprimée en pieds = ft), celles-ci ont été reprises depuis la carte VAC de BSL.
La structure utilisée pour un point de report est la suivante :
```
struct pt_report{
  char indicatif[LONGMAX_INDICATIF_PT_REPORT+1];
  int alt_pref; //en pieds
  int nb_avions; //nombre d'avions ayant le point de report comme derniere position
}
```

#### Itinéraire ####
Un itinéraire est composé d'une séquence de points de report. Nous considérerons ici les itinéraires dans le sens BSL -> extérieur, qu'il suffit d'inverser pour obtenir le symétrique extérieur -> BSL dans le cas d'une arrivée à Bâle-Mulhouse.
Comme il peut être utile de connaître le nombre d'avions ayant ce point de report comme dernier passé, la structure contient le compte des avions qui y sont passés dernièrement. Voici la structure :
```
struct itineraire{
  pt_report points[LONGMAX_ITINERAIRE];
}
```
L'aéroport de BSL compte trois itinéraires préférentiels publiés sur la carte VAC :
* E-NE-N
* E-S
* WB-WA-W

#### Aéroport ####
Le sujet impose de traiter le problème avec 20 aéroports français et 10 aéroports européens, tous distincts. J'ai choisi de considérer les 20 (respectivement 10) aéroports les fréquentés en France (resp. en Europe) hors BSL (soit 31 aéroports distincts au total).
La structure implémentée est la suivante :
```
struct aeroport{
  pays implantation; //pays ou est implante l'aeroport
  char nom[LONGMAX_NOM_AEROPORT+1]; //nom complet de l'aeroport
  char iata[LONGMAX_IATA+1];
  char suffixe_oaci[LONGMAX_SUFFIXE_OACI+1];
  itineraire itin_pref; //itineraire preferentiel pour rejoindre l'aeroport dans le sens BSL->exterieur
  int distance; //distance par rapport a BSL, exprimee en miles
}
```
#### Avion ####
L'avion, qui sera représenté par un ***processus***, possèdera la structure suivante :
```
struct avion{
  char suffixe_immat[LONGMAX_SUFFIXE_IMMAT+1]; //suffixe de l'immatriculation de l'avion (prefixe donne par le pays de l'aeroport ci-dessous)
  aeroport dest_arr; //pays de destination ou d'arrivee, peut etre BSL (cas du vol local BSL->BSL)
  int type_aero_ext; //vaut 0 si l'aeroport ci-dessus est celui d'arrivee (depart = BSL), 1 si c'est l'inverse
  int type_avion; //0 pour un léger, 1 pour un moyen porteur, 2 pour un gros porteur
  int etat; //0 : au sol, 1 : en vol, 2 : en détresse
  pt_report dern_pos; //dernier point de report passe par l'avion, vide s'il est au sol
  int code_transp; //code transpondeur affiche par l'avion
  int qte_carbu; //quantite de carburant actuellement dans l'avion
  int seuil_panne_carbu; //seuil a partir duquel le signal de panne de carburant sera envoye
  int vitesse_croisiere; //en noeuds
  struct tm ETA; //heure d'arrivee estimee en fonction de la distance de l'aeroport et de la vitesse de croisiere
}
```

### Eléments générés aléatoirement ###
Pour dynamiser le programme, les éléments suivants seront générés aléatoirement au lancement du programme
* Vent (direction et vitesse)  
* Pistes en service : parmi les 4 pistes 33L, 33R, 15R, 15L, deux pistes (dites "en service") sont retenues en fonction de la direction du vent (un avion décolle et atterrit toujours face au vent). Chaque piste en service sera associée à un ***sémaphore*** pour garantir qu'elle n'est occupée que par un seul avion à la fois.  
Chaque avion (= ***processus***) est généré aléatoirement tout au long de l'exécution du programme, selon les règles suivantes :  
* Son aéroport de départ ou (exclusif) d'arrivée est BSL  
  * s'il s'agit de son aéroport de départ, l'avion suivra l'itinéraire préférentiel associé à son aéroport d'arrivée  
  * sinon, il suivra ce même itinéraire mais dans l'ordre inverse
* A chaque point de report passe, sa quantité de carburant à bord diminue d'un delta arbitraire. Si le seuil d'alerte est passé 

### Eléments communicant entre eux ###
L'aéroport de Bâle-Mulhouse est un aéroport contrôlé. Cela sigifie que contrairement à d'autres zones dites en "auto information" où les pilotes se parlent entre eux (Habsheim - LFGB par exemple), toutes les communications passent ici par un ou plusieurs contrôleur(s) aérien(s). Dans notre cas (plusieurs contrôleurs agissent en simultané), ceux-ci utiliseront une fréquence propre (nous utiliserons informatiquement des ***files de messages***) selon la catégorie de la communication à établir, parmi :
* <span style="color:red;">Rouge</span> pour la TWR (tour)
* <span style="color:yellow;">Jaune</span> pour l'APP (approche)
* <span style="color:green;">Vert</span> pour le GND (sol)
* <span style="color:violet;">Violet</span> pour l'ATIS (message d'information)  
  * L'ATIS (Automatic Information Terminal Service) est un message vocal répété en boucle sur la fréquence associée, qui permet aux avions de prendre connaissance des paramètres actuels (météo, piste en service, calage altimétrique, etc...) avant le premier contact avec l'APP (arrivée)/ le GND (départ) pour éviter d'occuper inutilement une fréquence. Dans notre programme, son fonctionnement sera reproduit dans un message de la forme suivane :
  ```
  Ici BALE Aeroport, Information Echo enregistree à 14:56 UTC
  Piste en service legers et moyens porteurs : 33L
  Piste en service gros porteurs : 33R
  Vent du 106 DEG 15 KT
  Visibilite OK
  Temperature 16 DEG C
  Point de rosee 14 DEG C
  Calage altimetrique QNH 1024 QFE 1019
  Confirmez Information Echo recue avant premier contact
  ```
Pour gagner en clarté, j'ai trouvé pertinent de colorer les sorties sur le terminal en fonction du communicant qui s'exprime. J'ai fixé les couleurs ci-dessus. Les avions utiliseront la couleur <span style="color:blue;">bleue</span>. Leurs communications sortantes seront toujours précédées par leur immatriculation pour permettre la distinction avion/avion.
De plus, j'ai rendu atomique l'opération d'impression sur la sortie standard (*printf*) en la protégeant par un sémaphore de type mutex. Cela permet d'empêcher les communicants de s'interrompre entre eux, et d'obtenir des sorties intercalées dans d'autres ("coupure de parole").

### Signaler un avion en détresse ###
Pour ce faire, j'ai décidé d'utiliser le ***signal*** *SIGUSR1* qui simule la situation de détresse (MAYDAY MAYDAY MAYDAY) d'un avion. Lorsqu'il est déclenché, le signal a pour effet de placer un avion au hasard (parmi ceux qui sont en vol) dans une situation de détresse. Il devient alors prioritaire pour un atterrissage d'urgence sur le terrain de BSL.  
<u>Note :</u>Un autre traitant de signal a été implanté pour *SIGINT* qui permet de nettoyer tous les objets IPC lorsque le programme est interrompu dans son exécution.

## Conclusion ##
## Sources ##
[Organisation de l'Aviation Civile Internationale](https://www.icao.int/about-icao/Pages/default.aspx)  
[International Air Transport Association](https://www.iata.org/)  
[Wikipédia : Liste des aéroports les plus fréquentés en Europe](https://fr.wikipedia.org/wiki/Liste_des_a%C3%A9roports_les_plus_fr%C3%A9quent%C3%A9s_en_Europe)  
[Wikipédia : Liste des aéroports les plus fréquentés en France](https://fr.wikipedia.org/wiki/Liste_des_a%C3%A9roports_les_plus_fr%C3%A9quent%C3%A9s_en_France)  
[Wikipédia : Utilisation des codes d'échappement ANSI](https://en.wikipedia.org/wiki/ANSI_escape_code#Unix-like_systems)
[Rédaction d'un Makefile](https://ensiwiki.ensimag.fr/images/e/eb/Makefile.pdf)

## Annexes ##
### <u>Annexe 1 :</u> Jeu d'essai avec 10 avions ###