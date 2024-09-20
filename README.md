# Orkestracija raspodijeljenim web aplikacijama na uklopljenim sustavima

Autor : Lucian Tin Udovičić

Studijski smjer: Sveučilišni preddiplomski studij informatika

Mentor: doc. dr. sc. Nikola Tanković

#### Sažetak

Rad predstavlja softverski okvir koji omogućuje razvoj softvera neovisno o temeljnoj arhitekturi mikrokontrolera primjenom suvremenih načina programiranja na uređajima s ograničenim računalnim resursima. Osim izvođenja aplikacija, rad će opisati kako se omogućuje orkestriranje raspodijeljenih web aplikacija na uklopljenim sustavima uz pomoć radnog okruženja opisanog u ovom radu. Pokazati će se kako se apstrahiranjem funkcionalnosti temeljnog hardvera mogu razviti univerzalne aplikacije koje omogućuje jednostavno prebacivanje između različitih hardverskih platformi. Razvoj softvera za mikrokontrolere tradicionalno je suočen s izazovima kao što su ograničena procesorska snaga i memorijski kapacitet, zbog čega su potrebna inovativna rješenja za optimalno korištenje resursa. U tom kontekstu, razvojem novog radnog okruženja za mikrokontrolere, istražit će se kako aplikacije u spremnicima i tehnologije poput WebAssemblyja omogućuju razvoj univerzalnih i izoliranih aplikacija koje se mogu učinkovito izvoditi na različitim resursno ograničenim uređajima. 

[Završni rad](https://github.com/wasmboxes/EspBase/blob/main/Rad/Zavrsni.pdf)

## ESPbase framework za orkestraciju raspodijeljenih web aplikacija na uklopljenim sustavima

### Postavljanje okruženja :

- Uputstva su pisana prvenstveno za Arch Linux no trebalo bi biti otp. isto za ostale distribucije
- Neki paketi ovise o drugim paketima npr. cmake, python3, gcc itd... ali sazna se tokom instalacije


### 1. VS Code i ESP-IDF

##### [VS Code](https://aur.archlinux.org/packages/visual-studio-code-bin)
- Ne VS Code OSS ili sl. nego bin od Microsofta
```
$ git clone https://aur.archlinux.org/visual-studio-code-bin.git
$ makepkg -si
```

- Dodati dodatke : [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [ESP-IDF](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)

##### [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/get-started/index.html)
- verzija mora biti 4.4, ne preuzeti sa PlatformIO-a nego rucno postaviti i dodati u ESP-IDF plugin za VS Code
- Preuzimanje i postavljanje verzije 4.4 :
```
$ mkdir -p ~/esp
$ cd ~/esp
$ git clone -b v4.4 --recursive https://github.com/espressif/esp-idf.git
$ cd ~/esp/esp-idf
$ ./install.sh esp32
# nije potrebno dodati putanju u PATH, dovoljno source-at i otvoriti VS Code u istom kontekstu
$ source export.sh
# Unutar ESP-IDF; VS Code plugin-a, dodati putanju instalirane verzije
```


### 2. xxd, curl, WABT, Emscripten ... za stvaranje spremnika

- [Emscripten](https://emscripten.org/docs/getting_started/downloads.html)
```
$ git clone https://github.com/emscripten-core/emsdk.git
$ cd emsdk
$ git pull
$ ./emsdk install latest
$ source ./emsdk_env.sh
```

- xxd, curl (xxd se nalazi unutar vim-a)
```
pacman -S vim curl
```

- [WABT](https://github.com/WebAssembly/wabt)
```
$ git clone --recursive https://github.com/WebAssembly/wabt
$ cd wabt
$ git submodule update --init
# koristi se cmake za build-anje
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
# unutar direktorija se nalaze naredbe wasm2wat, wasm-objdump, wat2wasm itd... koje se koriste za provjeru i izmjenu wasm koda
```

- Na kraju moraju biti dostupne naredbe get_idf, cmake, emcc, gcc, wasm2wat, xxd i curl; mora takoder biti moguce kompajlat projekt unutar VS Code-a

### 3. Pokretanje

- Povezati esp32 ili esp32-c3 na racunalo
- Odabrati port i programator
- Postaviti WIFI login unutar main.c (predlaze se da se pusti dhcp i locka ip na routeru na MAC adresu chipa)
- Stisnuti "ESP-IDF : Build, Flash, Monitor"
- Cekati dok se ne boot-a i poveze na wifi
- Defaultno dolazi sa par spremnika koji se automatski ucitaju
- Naredba za dohvat popisa spremnika :
```
$ curl "http://<ip adresa>:10001/wasm/box/list"
# Trebao bi se vratiti JSON popis svih dostupih spremnika
```


