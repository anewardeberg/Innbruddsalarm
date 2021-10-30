# Innbruddsalarm (Arbeidskrav 2 - PG5501)
`Default PIN: 0000`

## Komponenter
| Komponent | Antall |
| ----------- | ----------- |
| Arduino Uno | 1 |
| Breadboard | 1 |
| Adafruit ST7789 | 1 |
| HC-SR501 | 1 |
| Membrane Switch Module 4x4 | 1 |
| DS3231 | 1 |
| Breadboard Jumper Wires | 22 |

### Adafruit 1.14" 240x135 Color TFT - ST7789
TFT-skjermen brukes som display for innbruddsalarmen. Størrelsen på 1.14" begrenser hvor mye informasjon som kan vises samtidig, men for dette prosjektet var det mer enn nok. Skjermen har høy ppi (pixel density), noe som gjør at skrift vises tydelig, selv om den kan være i det minste laget. Siden jeg ikke benyttet meg av SD-kort-slot'en til skjermen, brukte jeg kun 4 av SPI-koblingene; SCK, MOSI, TFTCS og DC. 

### HC-SR501 PIR Motion Sensor
Dersom innbruddsalarmen er plassert på en dør, er det tenkt at bevegelsessensoren skal detektere bevegelse, og igangstarte en alarmprosess som kun kan avbrytes av en satt kode. 

### Membrane Switch Module 4x4
Membrane Switch modulen brukes for inntasting og endring av kode for alarmen. I utgangspunktet har den et totalt antall av 16 knapper, men i dette prosjektet har jeg kun brukt 12 av dem. Dette er både på grunn av logistikkproblemer med at jeg ikke hadde nok pins tilgjengelig for å bruke alle 16, men også at jeg ikke så helt vitsen ved å bruke bokstavknappene. 

### DS3231 RTC Module
DS3231 RTC-modulen brukes for å holde styr på klokkefunksjonen som vises på TFT-skjermen. På grunn av dens I2C interface, kan den enkelt kobles på hvilken som helst mikrokontroller, og operere med enten 24 timers, eller 12 timers klokke. 

## Funksjonalitet
### Detekter bevegelse
Løsningen ber bruker om å skrive inn kode for å deaktivere alarmen så snart den merker bevegelse. Når bruker har skrevet inn koden, vil alarmen deaktiveres, og hjemskjermen vil vise status på alarmen; om den er aktivert eller ikke. 

### Utløse alarm dersom kode skrives inn feil 3 ganger på rad
Koden er skrevet slik at bruker har 3 forsøk på å skrive inn riktig kode. Dersom bruker har hatt 3 feilet forsøk, vil alarmen utløses. Per nå er det ikke lagt til noe funksjon for å komme seg ut av denne alarmen da det heller ikke er lagt til noen annen konsekvens av utkøst alarm annet enn en blinkende skjerm. For videre utvikling kunne alarmen for eksempel ha sendt et varsel til en registrert bruker om at alarmen er utløst. 

### Oppdatert klokke
På bunnen av skjermen vises en synkronisert klokke som refresher seg med intervaller på 10 sekunder for å vise så presis tid i timer og minutter som mulig, samtidig som at den ikke står og blinker hvert sekund. Dette er gjort ved hjelp av DS3231-chippen. 

### Mulighet for å endre kode
Når alarmen er deaktivert har bruker mulighet til å endre PIN-koden. Dette gjøres ved å trykke på `#`. Bruker blir deretter bedt om å skrive inn gammel kode, for så å komme opp med en ny kode, og bekrefte ved å skrive inn ny kode en gang til. 

### Reaktivere alarm 
Når bruker har deaaktivert alarmen, vil den også få mulighet til å reaktivere den ved å trykke på `*`. Ved inntasting av riktig kode, får brukeren 30 sekunder på seg før bevegelsessensoren begynner å registrere bevegelse i området på nytt. 


## Koblingsskjema
