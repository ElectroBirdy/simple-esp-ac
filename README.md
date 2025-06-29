# ESP32 DIY Klimatska Naprava z NTC Senzorji

Eksperimentalni projekt, ki uporablja ESP32 za nadzor predelane toplotne črpalke iz sušilnega stroja za hlajenje prostora. Sistem vključuje spremljanje temperature s pomočjo 10k NTC senzorjev, spletni uporabniški vmesnik, nadzor nad releji ter varnostne funkcije.

## Funkcionalnosti

- 📡 **Wi-Fi dostopna točka**: ESP32 ustvari lastno Wi-Fi omrežje, preko katerega dostopaš do spletne nadzorne plošče.
- 🌡️ **Merjenje temperature**: Prek 4 analognih NTC senzorjev (10k) se merijo:
  - Temperatura prostora
  - Temperatura kondenzatorja
  - Temperatura evaporatorja
  - Temperatura kompresorja
- 🔧 **Nastavljiva ciljna temperatura**: V uporabniškem vmesniku lahko nastaviš želeno temperaturo prostora.
- 🔁 **Avtomatski nadzor hlajenja**:
  - Vklop kompresorja in ventilatorjev dokler sobna temperatura ne doseže ciljne.
  - Izklop kompresorja, če se kondenzator ohladi pod 10 °C.
  - Dodatno delovanje ventilatorja na evaporatorju še 10 minut po izklopu hlajenja.
- 🔥 **Varnostna zaščita**:
  - Če se kompresor pregreje, se vklopi dodatni hladilni ventilator.
  - Če preseže kritično temperaturo, se kompresor ugasne do ohladitve.
- 🖥️ **Ročni nadzor relejev**: Vsak rele lahko vklopiš ali izklopiš neposredno s spletne strani.

## Potrebna strojna oprema

- ESP32 (WROOM)
- 4x 10k NTC temperaturni senzorji
- 4x analogni vhodi (ADC)
- 4x rele (npr. z rele moduli)
- Toplotna črpalka iz sušilnega stroja
- Napajanje 5V/3.3V za senzorje in ESP32

## Priključki senzorjev in relejev

| Funkcija               | ESP32 Pin | Opis                      |
|------------------------|-----------|---------------------
