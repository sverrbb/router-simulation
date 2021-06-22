/***********************************
 --- IN2140 OBLIG2 - RUTERDRIFT ----
 ***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>



/***** GLOBALE VARIABLER, LISTE OG KONSTANTER  *******
 * rutere - globale liste (dobbelpeker) for lagring av rutere
 * variabel N - global variabel for antall rutere i listen
 * Index_counter - variabel for innsetting av ruter i liste
 * MAX_KOBLINGER - max antall koblinger en ruter kan ha
 */
int N;
int index_counter = 0;
struct ruter **rutere;
#define MAX_KOBLINGER 10



/**
 * Struct ruter med tilhorende egenskaper
 * Array koblinger med pekere til alle koblinger
 */
struct ruter {
  // Egenskaper til ruter
  int ruterId;
  unsigned char flagg;
  char strengLengde;
  char *modell;

  // Koblinger til andre rutere
  struct ruter* koblinger[MAX_KOBLINGER];

  // Variabel brukt i grafsok
  bool besokt;
};




/**
 * Frigjor allokert minne for modellnavn i ruter
 * Frigjor allokert minne for struct ruter
 */
void free_ruter(struct ruter *ruter){
  free(ruter->modell);
  free(ruter);
}



/**
 * Frigjor plass for alle rutere i global liste med free_ruter()
 * Frigjor allokert minne for global liste rutere
 */
void free_rutere(){
  for(int i = 0; i < N; i++){
    if(rutere[i] != NULL){
      free_ruter(rutere[i]);
    }
  }
  free(rutere);
}



/**
 * Fyller/initialiserer variabler i struct ruter
 * Kalles fra les_fil() for aa opprette datastruktur
 */
void legg_til_ruter(int id, char *modell, unsigned char flagg, char lengde){

  // Allokerer plass til ruter og oppretter peker
  struct ruter *ruter = malloc(sizeof(struct ruter));
  if (ruter == NULL) {
      fprintf(stderr, "malloc: klarte ikke allokere i legg_til_ruter()\n");
      free_rutere();
      exit(EXIT_FAILURE);
  }

  // Allokerer plass og initialiserer variabel modell
  ruter->modell = strdup(modell);
  if (ruter->modell == NULL) {
      perror("strdup");
      free_rutere();
      free(ruter);
      exit(EXIT_FAILURE);
  }

  // Initialiserer variabler i ruter
  ruter -> ruterId = id;
  ruter -> flagg = flagg;
  ruter -> strengLengde = lengde;
  ruter -> besokt = false;

  // Setter alle koblinger til NULL
  for(int i = 0; i < MAX_KOBLINGER; i++){
      ruter -> koblinger[i] = NULL;
  }

  // Legger til ruter i global liste
  rutere[index_counter] = ruter;
  index_counter++;

}



/**
 * Henter og returnerer ruter paa oppgitt id
 * Brukes i kommando-metoder for aa hente ut pekere
 */
struct ruter *hent_ruter(int id) {
  for (int i = 0; i < N; i++) {
    if (rutere[i] != NULL && (rutere[i]->ruterId == id)) {
        return rutere[i];
    }
  }
  return NULL;
}



/**
 * Legger til kobling fra ruter id1 til id2
 * Kalles fra les_fil ved innlesing
 */
void fyll_koblinger (int id1, int id2) {

  // Henter rutere hvor det skal legges til kobling
  struct ruter *r1 = hent_ruter(id1);
  if(r1 == NULL){ // Avbryter hvis ruter ikke finnes
    printf(" - Fant ikke ruter med id: %d \n", id1);
    printf(" - Kobling fra ruter %d til ruter %d ble ikke satt\n", id1, id2);
    return;
  }

  // Henter ruter som det skal kobles til
  struct ruter *r2 = hent_ruter(id2);
  if(r2 == NULL){ // Avbryter hvis ruter ikke finnes
    printf(" - Fant ikke ruter med id: %d \n", id2);
    printf(" - Kobling fra ruter %d til ruter %d ble ikke satt\n", id1, id2);
    return;
  }

  // Legger inn kobling paa ledig plass (oppdaterer ruterstructen som er fra-siden i en kobling)
  for(int i = 0; i < MAX_KOBLINGER; i++){
    if(r1->koblinger[i] == NULL){
      r1->koblinger[i] = r2;
      break;
    }
  }
}



/**
 * Skriver ut bits
 * Brukes til aa skrive ut bits i flagg-byten
 * int start og stopp brukes for aa velge hvilke bit(s) som skal skrives ut
 */
void print_bits(void *nummer, int storrelse, int start, int stopp){
  //Itererer gjennom valgte bitposisjoner i byte og skriver ut
  unsigned char *til_bits = (unsigned char *) nummer;
  for(int x = storrelse-1; x >= 0; x--){
    for(int y = start; y >= stopp; y--){
      printf("%c", (til_bits[x] & (1 << y)) ? '1' : '0');
    }
    printf(" ");
  }

  // Printer desimaltall for flaggbyte eller endringsnummer i parentes
  if((start == 7 && stopp == 0) || (start == 3 && stopp == 0)){
    printf(" (%d)\n", *til_bits);
  }else{
    printf("\n");
  }
}



/**
 * Leser inn fra binaerfil
 * Lagrer data i struct ruter og legger
 * -til i global liste rutere med metoden legg_til_ruter()
 * filnavn - navn paa fil som skal leses inn, hentes fra argv[] i Main
 */
void les_fil(char *filnavn) {
    FILE *fil;
    int id;
    unsigned char flagg;
    char strengLengde;
    char modell[249];
    int Rt1;
    int Rt2;
    int rc;

    // Aapner binaerfil
    fil = fopen(filnavn, "rb");
    if (fil == NULL) {
        perror("fopen");
        free_rutere();
        exit(EXIT_FAILURE);
    }

    // Leser inn antall rutere
    fread(&N, sizeof(int), 1, fil);
    printf("\n**** TOTALT ANTALL RUTERE: %d ****\n", N);
    printf("‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");


    //Allokerer plass til N rutere
    rutere = (struct ruter**) malloc( N * (sizeof(struct ruter*)));
    if (rutere == NULL) {
        fprintf(stderr, "malloc: klarte ikke allokere i les_fil()\n");
        exit(EXIT_FAILURE);
    }

    // Leser inn informasjon om ruter og lagrer i struct
    for(int i = 0; i < N; i++){
      fread(&id, sizeof(int),1,fil);
      fread(&flagg,sizeof(char),1,fil);
      if (fread(&strengLengde, sizeof(char), 1, fil) == 0) break;
      rc = fread(modell, sizeof(char),strengLengde, fil);
      if (rc != strengLengde) break;
      modell[(int)strengLengde] = 0;
      legg_til_ruter(id, modell, flagg, strengLengde);
      fseek(fil, 1, SEEK_CUR);
    }

    // Leser- og legger til koblinger
    while(fread(&Rt1,sizeof(int),1,fil)){
          fread(&Rt2,sizeof(int),1,fil);
          fyll_koblinger(Rt1, Rt2);
    }

    // Sjekker for feil
    if (ferror(fil)) {
        fprintf(stderr, "fread failed\n");
        free_rutere();
        exit(EXIT_FAILURE);
    }

    //Lukker fil
    fclose(fil);
}



/**
 * Skriver ut bit-posisjoner i flagg som beskriver egenskaper til ruter
 * Bruker print_bits til aa velge start- og stopp bit, slik at man kan
 * velge hvilke bits i byte som skal skrives ut. Kalles fra print().
 */
void print_flagg(unsigned char *flagg){
  // Aktiv status: bitposisjon 0
  printf("  - Aktiv: ");
  print_bits(flagg, sizeof(char), 0, 0);

  // Traadlos: bitposisjon 1
  printf("  - Traadlos: ");
  print_bits(flagg, sizeof(char), 1, 1);

  // Stotte for 5GHz: bitposisjon 2
  printf("  - 5GHz: ");
  print_bits(flagg, sizeof(char), 2, 2);

  // Endringsnummer: bitposisjon 4-7
  printf("  - Endringsnummer: ");
  print_bits(flagg, sizeof(char), 7, 4);
}




/**
 * Skriver ut informasjon om ruter
 * Skriver ut alle koblinger til og fra ruter
 */
void print(int id){
  printf("\n\n**** Printer rute ****");

  // Henter ruter med id
  struct ruter *r = hent_ruter(id);
  if(r == NULL){
    printf(" - Fant ikke ruter med id: %d \n", id);
    return;
  }

  // Skriver ut informasjon om ruter
  printf("\nID: %d\n", r -> ruterId);
  printf("FLAGG: ");
  print_bits(&r -> flagg, sizeof(char), 7, 0);
  print_flagg(&r -> flagg);
  printf("Lengde: %d\n", r -> strengLengde);
  printf("Modell: %s\n", r -> modell);
  printf("Koblinger fra ruter:\n");

  // Skriver ut koblinger fra ruter
  for(int i = 0; i < MAX_KOBLINGER; i++){
    if(r -> koblinger[i] != NULL){
      printf("  - %d:", r -> koblinger[i] -> ruterId);
      printf(" %s\n", r -> koblinger[i] -> modell);
    }
  }

  // Skriver ut id til alle rutere med koblinger til ruter
  printf("Koblinger til ruter:\n");
  for(int k = 0; k < N; k++){
    for(int l = 0; l < MAX_KOBLINGER; l++){
      if(rutere[k] -> koblinger[l] != NULL){
        if(rutere[k] -> koblinger[l] -> ruterId == id){
            printf("  - %d:", rutere[k] -> ruterId);
            printf(" %s\n", rutere[k] -> modell);
            break;
        }
      }
    }
  }
  printf("\n");
}



/**
 * Skriver ut informasjon om alle rutere i den globale listen
 * Brukes for feilsjekking eller dersom Main ikke inneholder kommandofil
 */
void print_rutere() {
  for(int i = 0; i < N; i++){
    print(rutere[i]->ruterId);
  }
}



/**
 * Endrer modellnavn til ruter
 * id - rute der modell skal endres
 * navn - navn som skal endres til
 */
void sett_modell(int id, char* navn){
  printf("\n\n*** Setter modell paa ruter %d ***\n", id);

  // Finner ruter med riktig id
  struct ruter *ruter = hent_ruter(id);
  if(ruter == NULL){
    printf(" - Fant ikke ruter med id: %d \n", id);
    return;
  }

  // Endrer modellnavn og strengLengde
  printf(" - Fra modell: %s\n", ruter -> modell);
  free(ruter -> modell);
  ruter -> modell = strdup(navn);
  ruter -> strengLengde = strlen(navn);
  printf(" - Til modell: %s\n\n", ruter -> modell);

}



/**
 * Brukes i endringsnummer for aa velge operasjon
 * PlUSS - adderer med eksisterende endringsnummer
 * SETT_NY - setter nytt endringsnummer
 */
enum type {
  PLUSS,
  SETT_NY
};



/**
 * Sjekker om bit skal flippes og utforer operasjon
 * Kalles fra sett_endringsnummer() for aa oppdatere endringsnummer
 * num - peker til flagg-byte som skal endres
 * ny - peker til oppdatert endringsnummer som skal settes inn i flagg-byte
 * index - byte-index som skal settes
 */
void sett_bit(unsigned char *num, unsigned char *ny, int index){
  //Dersom 1 skal settes til null
  if(*num & (1 << index)){
    if(!(*ny & (1 << index))){
      *num &= ~(1 << index);
    }
  }

  //Dersom 0 skal settes til 1
  if(!(*num & (1 << index))){
    if(*ny & (1 << index)){
      *num |= (1 << index);
    }
  }
}



/**
 * Gjor operasjoner paa endringsnummer - dvs. bit 4 - 7 i flagg-byte
 * unsigned char *i - peker til flagg-byte som skal endres
 * enum type mode - type operasjon som skal gjennomfores (addisjon eller ny verdi)
 * unsigned char verdi - verdi som flagg skal oppdateres med
 */
void sett_endringsnummer(unsigned char *i, enum type mode, unsigned char verdi){
  printf("    - Fra endringsnummer: ");
  int sjekk_max = 0;

  // Henter bits som skal endres og flytter dem 4 hakk til venstre
  unsigned char temp = *i >> 4;

  // Sjekker at bits til venstre for endringsnummer er 0
  temp &= ~(1 << 4);
  temp &= ~(1 << 5);
  temp &= ~(1 << 6);
  temp &= ~(1 << 7);

  // Skriver kun ut endringsnummer (altsaa ikke resten av flagg-byte)
  print_bits(&temp, sizeof(char), 3, 0);

  // Adderer eller setter nytt endringsnummer
  switch(mode){
    case PLUSS:
      temp += verdi;
      break;
    case SETT_NY:
      temp = verdi;
      break;
  }

  // Sjekker om endringsnummer har naadd maxverdi som er 15 for 4 bits
  // Setter i saa fall endringsnummer til 0
  if(temp > 15){
    temp = 0;
    sjekk_max = 1;
  }

  // Skriver kun ut endringsnummer (altsaa ikke resten av flagg-byte)
  printf("    - Til endringsnummer: ");
  print_bits(&temp, sizeof(char), 3, 0);
  if(sjekk_max){
    printf("    - Ruter har naad max endringsnummer og blir derfor satt til 0\n");
  }

  // Flytter endringsnummer 4 hakk til venstre
  temp = temp << 4;

  // Legger tilbake oppdatert endringsnummer i flagg-byten
  for(int x = 4; x < 8; x++){
    sett_bit(i, &temp, x);
  }

}



/**
 * Oppdaterer flagg til ruter
 * Flagg 0 - er ruteren i bruk?
 * Flagg 1 - er ruteren traadlos?
 * Flagg 2 - stotter ruteren 5HGz?
 * Flagg 4-7 - endringsnummer til ruter
 */
void sett_flagg(int id, unsigned char flagg, unsigned char verdi) {
  printf("\n\n**** Setter flagg %d til verdi %d ****\n", flagg, verdi);

  // Finner ruter med riktig id
  struct ruter *ruter = hent_ruter(id);
  if(ruter == NULL){
    printf(" - Fant ikke ruter med id: %d \n", id);
    return;
  }

  // Skriver ut naavaerende flagg
  unsigned char flagg_kopi = ruter -> flagg;
  printf(" - Fra flagg: ");
  print_bits(&ruter -> flagg, sizeof(char), 7, 0);

   // Sjekker flagg 0, 1 og 2 og setter verdi
   if (flagg == 0 || flagg == 1 || flagg == 2) {

     // Setter flagg til verdi 0
     if (verdi == 0) {
         ruter -> flagg &= ~(1 << flagg);

     // Setter flagg til verdi 1
     } else if(verdi == 1) {
         ruter -> flagg |= (verdi << flagg);

     // Avbryter dersom flagg = 0, 1 eller 2 og verdi != 0 eller 1. MAO. ugyldig verdi
     } else{
         printf(" - Feil ved oppdatering av flagg: %d\n", flagg );
         printf(" - Flagg kan ikke settes til andre verdier enn 0 eller 1. Flagg ble ikke oppdatert\n");
         return;
     }

   // Sjekker om endringsnummer skal oppdaterers
   // Alle bitposisjoner for endringsnummer regnes som gyldig input
 } else if(flagg == 4 || flagg == 5 || flagg == 6 || flagg == 7){

        // Hvis verdi settes (SETT_NY) storre enn 15 returnerer funksjonen og kommando ignoreres
        // Hvis derimot verdi er 15 og funksjonen plusser (PLUSS) med 1 settes endringsnummer til 0
        // Dette haanderes i funksjonen sette_endringsnummer()
       if(verdi > 15){
         printf(" - Feil ved oppdatering av flagg: %d\n", flagg );
         printf(" - Endrinsnummer kan ikke ha storre verdi enn 15 og ble ikke oppdatert\n");
         return;
       }
       sett_endringsnummer(&ruter -> flagg, SETT_NY, verdi);

   // Dersom flagg == 3 eller flagg > 7
   } else{
    printf(" - Ugyldig input av flagg. Flagg nr. %d brukes ikke\n", flagg);
   }

  // Skriver ut oppdatert flagg
  printf(" - Til flagg: ");
  print_bits(&ruter -> flagg, sizeof(char), 7, 0);

  // Gir tilbakemelding dersom flagget allerede har input verdi
  if(ruter -> flagg == flagg_kopi){
    printf(" - Flagg %d har allerede verdien %d. Ingen endringer gjort\n\n", flagg, verdi);
  }
}



/**
 * Legger til kobling fra id1 til id2
 * Kalles gjennom kommando-fil
 */
void legg_til_kobling(int id1, int id2){
  printf("\n\n*** Legger til kobling fra %d til %d ***\n", id1, id2);

  // Henter ruter med id1
  struct ruter *r1 = hent_ruter(id1);
  if(r1 == NULL){
    printf(" - Fant ikke ruter med id: %d \n", id1);
    return;
  }

  // Henter ruter med id2
  struct ruter *r2 = hent_ruter(id2);
  if(r2 == NULL){
    printf(" - Fant ikke ruter med id: %d \n", id2);
    return;
  }

  // Legger til kobling fra id1 til id2
  for(int i = 0; i < MAX_KOBLINGER; i++){
    if(r1 -> koblinger[i] == NULL){
      r1 -> koblinger[i] = r2;
      printf(" - Oppdaterer endringsnummer for ruter %d\n", r1 -> ruterId);
      sett_endringsnummer(&r1 -> flagg, PLUSS, 1);
      printf(" - Kobling fra ruter %d til ruter %d ble lagt til\n\n", id1, id2);
      return;
    }
  }
}



/**
 * Funkson for aa nullstille en ruters besokt-status ved
 * grafsok dersom sok skal gjennomfores glere ganger.
 * Endrer besokt variabel til false for alle rutere
 */
void gjor_ubesokt(){
  for(int i = 0; i < N; i++){
    if(rutere[i] != NULL){
      rutere[i] -> besokt = false;
    }
  }
}



/**
 * Sjekker om det finnes en kobling mellom to rutere
 * id1 - id til ruter som det skal sokes fra
 * id2 - id til ruter som det skal sokes til
 */
bool finnes_rute(int id1, int id2){

    // Henter ruter med id1
    struct ruter *r1 = hent_ruter(id1);
    if(r1 == NULL){
      printf(" - Fant ikke ruter med id: %d \n", id1);
      return false;
    }

    // Henter ruter med id2
    struct ruter *r2 = hent_ruter(id2);
    if(r2 == NULL){
      printf(" - Fant ikke ruter med id: %d \n", id2);
      return false;
    }

    // Printer ut ruterId for aa vise graf
    if(r1 -> besokt == false){
      printf("%d --> ", r1 -> ruterId);
    }

    // Registrerer ruter som besokt
    r1 -> besokt = true;
    if(r1 == r2){
        return true;
    }else{
      for(int i = 0; i < MAX_KOBLINGER; i++){
        if(r1 -> koblinger[i] != NULL && r1 -> koblinger[i] -> besokt == false){
          if(r1 -> koblinger[i] == r2){
            return true;
          } else{
              if(r1 -> koblinger[i] != NULL){
                if(finnes_rute(r1 -> koblinger[i] -> ruterId, id2) == true){
                    return true;
                }
              }
          }
        }
      }
    }
  return false;
}



/**
 * Sletter ruter med param id
 * Sletter alle pekere til ruter
 */
void slett_rute(int id){
  printf("\n\n*** Prover aa slette ruter med id: %d ***\n", id);

  // Fjerner pekere til ruter
  for(int i = 0; i < N; i++){
    for(int j = 0; j < MAX_KOBLINGER; j++){
      if(rutere[i] -> koblinger[j] != NULL){
        if(rutere[i] -> koblinger[j] -> ruterId == id){
          rutere[i] -> koblinger[j] = NULL;

          // Oppdaterer endringsnummer til rutere som er berort av slettingen
          // Det vil si alle rutere som har en peker / kobling til ruteren som skal slettes
          printf(" - Oppdaterer endringsnummer for ruter: %d\n", rutere[i] -> ruterId);
          sett_endringsnummer(&rutere[i] -> flagg, PLUSS, 1);
          break;
        }
      }
    }
  }

  // Fjerner ruter
  for(int k = 0; k < N; k++){
    if(rutere[k] -> ruterId == id){
      free_ruter(rutere[k]);
      rutere[k] = NULL;

      // Samler global liste og tetter "hull"
      for(int l = 0; l < N; l++) {
         if (rutere[l] == NULL && l < N-1) {
           for(int m = l + 1; m < N; m++) {
               rutere[l] = rutere[m];
               l++;
           }
           rutere[l] = NULL;
         }
      }
      printf(" - Ruter ble slettet\n\n");

      // Reduserer antall rutere i global liste med 1
      N = N - 1;
      return;
    }
  }
  printf(" - Ruter ble ikke slettet. Fant ingen ruter med id %d\n\n", id);
}



/**
 * Leser inn kommando-fil
 * Kaller paa kommandoer i fil
 */
void les_fil2(char *filnavn){
  char kommando[255];
  char *file_string;

  // Aapner fil
  FILE *fp;
  fp = fopen(filnavn, "r");
  if (fp == NULL) {
      perror("fopen");
      free_rutere();
      exit(EXIT_FAILURE);
  }

  //Leser en og en linje fra kommando-fil
  while (fgets(kommando, 255, fp) != NULL) {
     gjor_ubesokt(); //Gjor alle rutere ubesokt
     file_string = strtok(kommando, " ");

     //Sammenligner kommando og kommandonavn - returnerer 0 om de er like
     int print_kommando = strcmp(file_string, "print");
     int flagg_kommando = strcmp(file_string, "sett_flagg");
     int modell_kommando = strcmp(file_string, "sett_modell");
     int kobling_kommando = strcmp(file_string, "legg_til_kobling");
     int slett_kommando = strcmp(file_string, "slett_ruter");
     int rute_kommando = strcmp(file_string, "finnes_rute");

     // Sjekker om kommando er print og kaller metode
     if (print_kommando == 0) {
         file_string = strtok(NULL, "\n");
         int id = (int) atoi(file_string);
         print(id);

     // Sjekker om kommando er sett_flagg og kaller metode
    } else if (flagg_kommando == 0) {
         file_string = strtok(NULL, " ");
         int id = (int) atoi(file_string);
         file_string = strtok(NULL, " ");
         unsigned char flagg = (unsigned char) atoi(file_string);
         file_string = strtok(NULL, "\n");
         unsigned char verdi = (unsigned char) atoi(file_string);
         sett_flagg(id, flagg, verdi);

     // Sjekker om kommando er sett_modell og kaller metode
     } else if (modell_kommando == 0) {
         file_string = strtok(NULL, " ");
         int id = (int) atoi(file_string);
         file_string = strtok(NULL, "\n");
         char* navn = file_string;
         sett_modell(id, navn);

     // Sjekker om kommando er legg_til_kobling og kaller metode
     } else if (kobling_kommando == 0) {
         file_string = strtok(NULL, " ");
         int id1 = (int) atoi(file_string);
         file_string = strtok(NULL, "\n");
         int id2 = (int) atoi(file_string);
         legg_til_kobling(id1, id2);

       // Sjekker om kommando er slett_ruter og kaller metode
     } else if (slett_kommando == 0) {
           file_string = strtok(NULL, "\n");
           int ruterID = (int) atoi(file_string);
           slett_rute(ruterID);

     // Sjekker om kommando er finnes_rute og kaller metode
     } else if (rute_kommando == 0) {
         file_string = strtok(NULL, " ");
         int id1 = (int) atoi(file_string);
         file_string = strtok(NULL, "\n");
         int id2 = (int) atoi(file_string);
         printf("\n\n*** Sjekker om kobling finnes fra %d til %d ***\n - Forbindelse: (", id1, id2);
         if (finnes_rute(id1, id2) == true) {
             printf("%u) \n - RESULTAT AV SØK: Rute finnes fra %u -> %u\n",id2, id1, id2);
             printf("                    ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");
         } else {
             printf("?)  \n - RESULTAT AV SØK: Rute finnes IKKE fra %u -> %u\n", id1, id2);
             printf("                    ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");
         }
      }
   }

  // Lukker fil
  fclose(fp);
}



/**
 * Skriver til fil
 * filename - navn paa fil som skal opprettes og skrives til
 * Skriver forst antall rutere N og deretter N antall informasjonsblokker om rutere
 * Skriver deretter alle koblingsinformasjonsblokker som den finner
 */
void skriv_til_fil(char *filename) {

    // Aapner fil
    FILE *file;
    int wc;
    file = fopen(filename, "wb");
    if (file == NULL) {
        perror("fopen");
        free_rutere();
        exit(EXIT_FAILURE);
    }

    // Skriver antall rutere i listen til fil
    fwrite(&N, sizeof(int), 1, file);

    // Skriver informasjon om hver ruter til fil
    for(int i = 0; i < N; i++){
      fwrite(&rutere[i] -> ruterId, sizeof(int),1,file);
      fwrite(&rutere[i] -> flagg, sizeof(char),1,file);
      if(fwrite(&rutere[i] -> strengLengde, sizeof(char), 1, file) == 0) break;
      wc = fwrite(rutere[i] -> modell, sizeof(char), rutere[i] -> strengLengde, file);
      if(wc != rutere[i] -> strengLengde) break;
      fwrite("\n", sizeof(char), 1, file);
    }

    // Skriver informasjon om koblinger til fil
    for(int j = 0; j < N; j++){
      for(int k = 0; k < MAX_KOBLINGER; k++){
        if(rutere[j] -> koblinger[k] != NULL){
          fwrite(&rutere[j]->ruterId, sizeof(int), 1, file);
          fwrite(&rutere[j]->koblinger[k]->ruterId, sizeof(int), 1, file);
        }
      }
    }

    // Sjekker for error
    if (ferror(file)) {
        fprintf(stderr, "fwrite failed\n");
        free_rutere();
        exit(EXIT_FAILURE);
    }

    //Lukker fil
    fclose(file);
}




/**
 * 1. Henter filnavn fra kommandolinje
 * 2. Leser inn binaerfil og lagrer dataen i global liste rutere
 * 3. Leser inn fra kommandofil og utforer kommandoer
 * 4. Skriver oppdatert data til binaerfil
 * 5. Frigjor allokert plass
 */
int main(int argc, char* argv[]) {

    // Sjekker at antall filer er riktig.
    // Det skal vere mulig aa kun sende inn binaerfil altsaa-
    // at argc == 2, og sjekker derfor kun for tilfeller der argc > 3 eller argc < 2
    if (argc > 3 || argc < 2) {
  		printf("Feil med input. Riktig bruk er:\n");
      printf(" - Alternativ 1: ./ruterdrift <binaerfil> <kommandofil>\n");
      printf(" - Alternativ 2: ./ruterdrift <binaerfil>\n");
  		return EXIT_FAILURE;
    }

    // Leser inn binaerfil
    char* filnavn = argv[1];


    // Leser inn kommandofil
    char* filnavn2 = argv[2];


    // Kaller paa les_fil som leser inn binaerfil
    // Leser med info om rutere og koblinger og lagrer globalt
    les_fil(filnavn);


    // Kaller paa les_fil2 som leser inn kommando-fil
    // Kalles kun hvis argc == 3, altsaa dersom kun tilfeller
    // -der baade binaerfil og kommandofil er argumenter
    if(argc == 3){
      les_fil2(filnavn2);
    }


    // Ekstra funksjonalitet som skriver ut info om alle rutere
    // Kalles bare dersom argc == 2, mao. tilfeller med kun binaerfil som argument
    if(argc == 2){
      print_rutere();
    }


    // Skriver info om rutere til fil
    // Lager nye binaerfil "new-topology.dat" som opprettes ved kallet
    // Dersom filen allerede eksisterer overskrides innholdet i filen
    skriv_til_fil("new-topology.dat");


    // Frigjor allokert minne i program
    free_rutere();


    return EXIT_SUCCESS;

}
