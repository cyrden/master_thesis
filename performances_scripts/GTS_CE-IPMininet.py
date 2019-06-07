#!/usr/bin/python

"""
Custom topology for IPMininet, generated by GraphML-Topo-to-IPMininet-Network-Generator.
"""
from mininet.log import lg

import subprocess
import ipmininet
from ipmininet.cli import IPCLI
from ipmininet.ipnet import IPNet
from ipmininet.iptopo import IPTopo
from ipmininet.link import IPIntf


class SimpleOSPFNet(IPTopo):

    def build(self, *args, **kwargs):


        # add nodes, routers first...
        Gorzow = self.addRouter_v4( 'r0' )
        ZeilonaGora = self.addRouter_v4( 'r1' )
        Kobenhavn = self.addRouter_v4( 'r2' )
        Hamburg = self.addRouter_v4( 'r3' )
        Gdansk = self.addRouter_v4( 'r4' )
        Gdynia = self.addRouter_v4( 'r5' )
        Slupsk = self.addRouter_v4( 'r6' )
        Koszalin = self.addRouter_v4( 'r7' )
        Kolobrzeg = self.addRouter_v4( 'r8' )
        Szczecin = self.addRouter_v4( 'r9' )
        Braniewo = self.addRouter_v4( 'r10' )
        Poti_Georgia = self.addRouter_v4( 'r12' )
        Georgia_Armenia_Iran_Iraq_Syria = self.addRouter_v4( 'r13' )
        Amsterdam_London = self.addRouter_v4( 'r15' )
        Lowicz = self.addRouter_v4( 'r18' )
        Ulmi = self.addRouter_v4( 'r19' )
        Beograd = self.addRouter_v4( 'r20' )
        Balvanesti = self.addRouter_v4( 'r21' )
        Alexandria = self.addRouter_v4( 'r22' )
        Craiova = self.addRouter_v4( 'r23' )
        Arad = self.addRouter_v4( 'r24' )
        Oradea = self.addRouter_v4( 'r25' )
        Caransebes = self.addRouter_v4( 'r26' )
        Timisoara = self.addRouter_v4( 'r27' )
        Ploiesti = self.addRouter_v4( 'r28' )
        Giurgiu = self.addRouter_v4( 'r29' )
        Kazakhstan = self.addRouter_v4( 'r30' )
        Szombathely = self.addRouter_v4( 'r31' )
        Szekesfehervar = self.addRouter_v4( 'r32' )
        Tatabanya = self.addRouter_v4( 'r33' )
        Keszthely = self.addRouter_v4( 'r34' )
        Veszprem = self.addRouter_v4( 'r35' )
        Rozvadov = self.addRouter_v4( 'r36' )
        KarlovyVary = self.addRouter_v4( 'r37' )
        Liberec = self.addRouter_v4( 'r38' )
        Plzen = self.addRouter_v4( 'r39' )
        Kyiv = self.addRouter_v4( 'r40' )
        Warszawa = self.addRouter_v4( 'r41' )
        Tirane = self.addRouter_v4( 'r42' )
        Athina = self.addRouter_v4( 'r43' )
        Vilnius = self.addRouter_v4( 'r44' )
        Tallinn = self.addRouter_v4( 'r45' )
        Kaliningrad = self.addRouter_v4( 'r46' )
        Minsk = self.addRouter_v4( 'r47' )
        Riga = self.addRouter_v4( 'r48' )
        Helsinki = self.addRouter_v4( 'r49' )
        Poieni = self.addRouter_v4( 'r50' )
        Sliven = self.addRouter_v4( 'r51' )
        Iasi = self.addRouter_v4( 'r52' )
        Galati = self.addRouter_v4( 'r53' )
        Brasov = self.addRouter_v4( 'r54' )
        Bacau = self.addRouter_v4( 'r55' )
        Medias = self.addRouter_v4( 'r56' )
        Cluj = self.addRouter_v4( 'r57' )
        Constanta = self.addRouter_v4( 'r58' )
        Dumbravita = self.addRouter_v4( 'r59' )
        Komarno = self.addRouter_v4( 'r60' )
        Gyor = self.addRouter_v4( 'r61' )
        Sopron = self.addRouter_v4( 'r62' )
        Odesa = self.addRouter_v4( 'r63' )
        Kharkiv = self.addRouter_v4( 'r64' )
        Dnipropetrovsk = self.addRouter_v4( 'r65' )
        Lviv = self.addRouter_v4( 'r66' )
        Tarnow = self.addRouter_v4( 'r67' )
        BanskaBystrica = self.addRouter_v4( 'r68' )
        NoveZamky = self.addRouter_v4( 'r69' )
        Praha = self.addRouter_v4( 'r70' )
        Szolnok = self.addRouter_v4( 'r71' )
        Zalaegerszeg = self.addRouter_v4( 'r72' )
        Tapolca = self.addRouter_v4( 'r73' )
        Tekirdag = self.addRouter_v4( 'r74' )
        Cheb = self.addRouter_v4( 'r75' )
        UstinadLabem = self.addRouter_v4( 'r76' )
        HradecKralove = self.addRouter_v4( 'r77' )
        Dunaujvaros = self.addRouter_v4( 'r78' )
        Kiskunhalas = self.addRouter_v4( 'r79' )
        Tompa = self.addRouter_v4( 'r80' )
        Szeged = self.addRouter_v4( 'r81' )
        Szentes = self.addRouter_v4( 'r82' )
        Zilina = self.addRouter_v4( 'r83' )
        Kosice = self.addRouter_v4( 'r84' )
        Kecskemet = self.addRouter_v4( 'r85' )
        Paks = self.addRouter_v4( 'r86' )
        Burgas = self.addRouter_v4( 'r87' )
        Debrecen = self.addRouter_v4( 'r88' )
        Bors = self.addRouter_v4( 'r89' )
        Haskovo = self.addRouter_v4( 'r90' )
        Wroclaw = self.addRouter_v4( 'r91' )
        Leszno = self.addRouter_v4( 'r92' )
        Inowroclaw = self.addRouter_v4( 'r93' )
        Poznan = self.addRouter_v4( 'r94' )
        Tczew = self.addRouter_v4( 'r95' )
        Bydgoszcz = self.addRouter_v4( 'r96' )
        Wloclawek = self.addRouter_v4( 'r97' )
        Olsztyn = self.addRouter_v4( 'r98' )
        Konin = self.addRouter_v4( 'r99' )
        Torun = self.addRouter_v4( 'r100' )
        Lublin = self.addRouter_v4( 'r101' )
        Rzeszow = self.addRouter_v4( 'r102' )
        Kutno = self.addRouter_v4( 'r103' )
        Bialystok = self.addRouter_v4( 'r104' )
        Radom = self.addRouter_v4( 'r105' )
        Lodz = self.addRouter_v4( 'r106' )
        Krakow = self.addRouter_v4( 'r107' )
        Kielce = self.addRouter_v4( 'r108' )
        Opole = self.addRouter_v4( 'r109' )
        Katowice = self.addRouter_v4( 'r110' )
        Svilengrad = self.addRouter_v4( 'r111' )
        Edirne = self.addRouter_v4( 'r112' )
        Olomouc = self.addRouter_v4( 'r113' )
        Hodonin = self.addRouter_v4( 'r114' )
        Zlin = self.addRouter_v4( 'r115' )
        Ostrava = self.addRouter_v4( 'r116' )
        Varna = self.addRouter_v4( 'r117' )
        CeskeBudejovice = self.addRouter_v4( 'r118' )
        Jihlava = self.addRouter_v4( 'r119' )
        Brno = self.addRouter_v4( 'r120' )
        Bielsko = self.addRouter_v4( 'r121' )
        Most = self.addRouter_v4( 'r122' )
        Ljubljana = self.addRouter_v4( 'r123' )
        Zagreb = self.addRouter_v4( 'r124' )
        FrankfurtamMain = self.addRouter_v4( 'r125' )
        Moskva = self.addRouter_v4( 'r126' )
        SanktPeterburg = self.addRouter_v4( 'r127' )
        Stockholm = self.addRouter_v4( 'r128' )
        Budapest = self.addRouter_v4( 'r129' )
        Bratislava = self.addRouter_v4( 'r130' )
        Wien = self.addRouter_v4( 'r131' )
        Berlin = self.addRouter_v4( 'r132' )
        Pristina = self.addRouter_v4( 'r133' )
        Podgorica = self.addRouter_v4( 'r134' )
        Balikesir = self.addRouter_v4( 'r135' )
        Izmir = self.addRouter_v4( 'r136' )
        Thessaloniki = self.addRouter_v4( 'r137' )
        Petrich = self.addRouter_v4( 'r138' )
        Pleven = self.addRouter_v4( 'r139' )
        Plovdiv = self.addRouter_v4( 'r140' )
        Sarajevo = self.addRouter_v4( 'r141' )
        Bucuresti = self.addRouter_v4( 'r142' )
        Chisinau = self.addRouter_v4( 'r143' )
        Sofia = self.addRouter_v4( 'r144' )
        Istanbul = self.addRouter_v4( 'r145' )
        Istanbul = self.addRouter_v4( 'r146' )
        Ankara = self.addRouter_v4( 'r147' )
        Skopje = self.addRouter_v4( 'r148' )

        # ... and now hosts
        Gorzow_host = self.addHost( 'h0' )
        ZeilonaGora_host = self.addHost( 'h1' )
        Kobenhavn_host = self.addHost( 'h2' )
        Hamburg_host = self.addHost( 'h3' )
        Gdansk_host = self.addHost( 'h4' )
        Gdynia_host = self.addHost( 'h5' )
        Slupsk_host = self.addHost( 'h6' )
        Koszalin_host = self.addHost( 'h7' )
        Kolobrzeg_host = self.addHost( 'h8' )
        Szczecin_host = self.addHost( 'h9' )
        Braniewo_host = self.addHost( 'h10' )
        Poti_Georgia_host = self.addHost( 'h12' )
        Georgia_Armenia_Iran_Iraq_Syria_host = self.addHost( 'h13' )
        Amsterdam_London_host = self.addHost( 'h15' )
        Lowicz_host = self.addHost( 'h18' )
        Ulmi_host = self.addHost( 'h19' )
        Beograd_host = self.addHost( 'h20' )
        Balvanesti_host = self.addHost( 'h21' )
        Alexandria_host = self.addHost( 'h22' )
        Craiova_host = self.addHost( 'h23' )
        Arad_host = self.addHost( 'h24' )
        Oradea_host = self.addHost( 'h25' )
        Caransebes_host = self.addHost( 'h26' )
        Timisoara_host = self.addHost( 'h27' )
        Ploiesti_host = self.addHost( 'h28' )
        Giurgiu_host = self.addHost( 'h29' )
        Kazakhstan_host = self.addHost( 'h30' )
        Szombathely_host = self.addHost( 'h31' )
        Szekesfehervar_host = self.addHost( 'h32' )
        Tatabanya_host = self.addHost( 'h33' )
        Keszthely_host = self.addHost( 'h34' )
        Veszprem_host = self.addHost( 'h35' )
        Rozvadov_host = self.addHost( 'h36' )
        KarlovyVary_host = self.addHost( 'h37' )
        Liberec_host = self.addHost( 'h38' )
        Plzen_host = self.addHost( 'h39' )
        Kyiv_host = self.addHost( 'h40' )
        Warszawa_host = self.addHost( 'h41' )
        Tirane_host = self.addHost( 'h42' )
        Athina_host = self.addHost( 'h43' )
        Vilnius_host = self.addHost( 'h44' )
        Tallinn_host = self.addHost( 'h45' )
        Kaliningrad_host = self.addHost( 'h46' )
        Minsk_host = self.addHost( 'h47' )
        Riga_host = self.addHost( 'h48' )
        Helsinki_host = self.addHost( 'h49' )
        Poieni_host = self.addHost( 'h50' )
        Sliven_host = self.addHost( 'h51' )
        Iasi_host = self.addHost( 'h52' )
        Galati_host = self.addHost( 'h53' )
        Brasov_host = self.addHost( 'h54' )
        Bacau_host = self.addHost( 'h55' )
        Medias_host = self.addHost( 'h56' )
        Cluj_host = self.addHost( 'h57' )
        Constanta_host = self.addHost( 'h58' )
        Dumbravita_host = self.addHost( 'h59' )
        Komarno_host = self.addHost( 'h60' )
        Gyor_host = self.addHost( 'h61' )
        Sopron_host = self.addHost( 'h62' )
        Odesa_host = self.addHost( 'h63' )
        Kharkiv_host = self.addHost( 'h64' )
        Dnipropetrovsk_host = self.addHost( 'h65' )
        Lviv_host = self.addHost( 'h66' )
        Tarnow_host = self.addHost( 'h67' )
        BanskaBystrica_host = self.addHost( 'h68' )
        NoveZamky_host = self.addHost( 'h69' )
        Praha_host = self.addHost( 'h70' )
        Szolnok_host = self.addHost( 'h71' )
        Zalaegerszeg_host = self.addHost( 'h72' )
        Tapolca_host = self.addHost( 'h73' )
        Tekirdag_host = self.addHost( 'h74' )
        Cheb_host = self.addHost( 'h75' )
        UstinadLabem_host = self.addHost( 'h76' )
        HradecKralove_host = self.addHost( 'h77' )
        Dunaujvaros_host = self.addHost( 'h78' )
        Kiskunhalas_host = self.addHost( 'h79' )
        Tompa_host = self.addHost( 'h80' )
        Szeged_host = self.addHost( 'h81' )
        Szentes_host = self.addHost( 'h82' )
        Zilina_host = self.addHost( 'h83' )
        Kosice_host = self.addHost( 'h84' )
        Kecskemet_host = self.addHost( 'h85' )
        Paks_host = self.addHost( 'h86' )
        Burgas_host = self.addHost( 'h87' )
        Debrecen_host = self.addHost( 'h88' )
        Bors_host = self.addHost( 'h89' )
        Haskovo_host = self.addHost( 'h90' )
        Wroclaw_host = self.addHost( 'h91' )
        Leszno_host = self.addHost( 'h92' )
        Inowroclaw_host = self.addHost( 'h93' )
        Poznan_host = self.addHost( 'h94' )
        Tczew_host = self.addHost( 'h95' )
        Bydgoszcz_host = self.addHost( 'h96' )
        Wloclawek_host = self.addHost( 'h97' )
        Olsztyn_host = self.addHost( 'h98' )
        Konin_host = self.addHost( 'h99' )
        Torun_host = self.addHost( 'h100' )
        Lublin_host = self.addHost( 'h101' )
        Rzeszow_host = self.addHost( 'h102' )
        Kutno_host = self.addHost( 'h103' )
        Bialystok_host = self.addHost( 'h104' )
        Radom_host = self.addHost( 'h105' )
        Lodz_host = self.addHost( 'h106' )
        Krakow_host = self.addHost( 'h107' )
        Kielce_host = self.addHost( 'h108' )
        Opole_host = self.addHost( 'h109' )
        Katowice_host = self.addHost( 'h110' )
        Svilengrad_host = self.addHost( 'h111' )
        Edirne_host = self.addHost( 'h112' )
        Olomouc_host = self.addHost( 'h113' )
        Hodonin_host = self.addHost( 'h114' )
        Zlin_host = self.addHost( 'h115' )
        Ostrava_host = self.addHost( 'h116' )
        Varna_host = self.addHost( 'h117' )
        CeskeBudejovice_host = self.addHost( 'h118' )
        Jihlava_host = self.addHost( 'h119' )
        Brno_host = self.addHost( 'h120' )
        Bielsko_host = self.addHost( 'h121' )
        Most_host = self.addHost( 'h122' )
        Ljubljana_host = self.addHost( 'h123' )
        Zagreb_host = self.addHost( 'h124' )
        FrankfurtamMain_host = self.addHost( 'h125' )
        Moskva_host = self.addHost( 'h126' )
        SanktPeterburg_host = self.addHost( 'h127' )
        Stockholm_host = self.addHost( 'h128' )
        Budapest_host = self.addHost( 'h129' )
        Bratislava_host = self.addHost( 'h130' )
        Wien_host = self.addHost( 'h131' )
        Berlin_host = self.addHost( 'h132' )
        Pristina_host = self.addHost( 'h133' )
        Podgorica_host = self.addHost( 'h134' )
        Balikesir_host = self.addHost( 'h135' )
        Izmir_host = self.addHost( 'h136' )
        Thessaloniki_host = self.addHost( 'h137' )
        Petrich_host = self.addHost( 'h138' )
        Pleven_host = self.addHost( 'h139' )
        Plovdiv_host = self.addHost( 'h140' )
        Sarajevo_host = self.addHost( 'h141' )
        Bucuresti_host = self.addHost( 'h142' )
        Chisinau_host = self.addHost( 'h143' )
        Sofia_host = self.addHost( 'h144' )
        Istanbul_host = self.addHost( 'h145' )
        Istanbul_host = self.addHost( 'h146' )
        Ankara_host = self.addHost( 'h147' )
        Skopje_host = self.addHost( 'h148' )

        # add edges between router and corresponding host
        self.addLink( Gorzow , Gorzow_host )
        self.addLink( ZeilonaGora , ZeilonaGora_host )
        self.addLink( Kobenhavn , Kobenhavn_host )
        self.addLink( Hamburg , Hamburg_host )
        self.addLink( Gdansk , Gdansk_host )
        self.addLink( Gdynia , Gdynia_host )
        self.addLink( Slupsk , Slupsk_host )
        self.addLink( Koszalin , Koszalin_host )
        self.addLink( Kolobrzeg , Kolobrzeg_host )
        self.addLink( Szczecin , Szczecin_host )
        self.addLink( Braniewo , Braniewo_host )
        self.addLink( Poti_Georgia , Poti_Georgia_host )
        self.addLink( Georgia_Armenia_Iran_Iraq_Syria , Georgia_Armenia_Iran_Iraq_Syria_host )
        self.addLink( Amsterdam_London , Amsterdam_London_host )
        self.addLink( Lowicz , Lowicz_host )
        self.addLink( Ulmi , Ulmi_host )
        self.addLink( Beograd , Beograd_host )
        self.addLink( Balvanesti , Balvanesti_host )
        self.addLink( Alexandria , Alexandria_host )
        self.addLink( Craiova , Craiova_host )
        self.addLink( Arad , Arad_host )
        self.addLink( Oradea , Oradea_host )
        self.addLink( Caransebes , Caransebes_host )
        self.addLink( Timisoara , Timisoara_host )
        self.addLink( Ploiesti , Ploiesti_host )
        self.addLink( Giurgiu , Giurgiu_host )
        self.addLink( Kazakhstan , Kazakhstan_host )
        self.addLink( Szombathely , Szombathely_host )
        self.addLink( Szekesfehervar , Szekesfehervar_host )
        self.addLink( Tatabanya , Tatabanya_host )
        self.addLink( Keszthely , Keszthely_host )
        self.addLink( Veszprem , Veszprem_host )
        self.addLink( Rozvadov , Rozvadov_host )
        self.addLink( KarlovyVary , KarlovyVary_host )
        self.addLink( Liberec , Liberec_host )
        self.addLink( Plzen , Plzen_host )
        self.addLink( Kyiv , Kyiv_host )
        self.addLink( Warszawa , Warszawa_host )
        self.addLink( Tirane , Tirane_host )
        self.addLink( Athina , Athina_host )
        self.addLink( Vilnius , Vilnius_host )
        self.addLink( Tallinn , Tallinn_host )
        self.addLink( Kaliningrad , Kaliningrad_host )
        self.addLink( Minsk , Minsk_host )
        self.addLink( Riga , Riga_host )
        self.addLink( Helsinki , Helsinki_host )
        self.addLink( Poieni , Poieni_host )
        self.addLink( Sliven , Sliven_host )
        self.addLink( Iasi , Iasi_host )
        self.addLink( Galati , Galati_host )
        self.addLink( Brasov , Brasov_host )
        self.addLink( Bacau , Bacau_host )
        self.addLink( Medias , Medias_host )
        self.addLink( Cluj , Cluj_host )
        self.addLink( Constanta , Constanta_host )
        self.addLink( Dumbravita , Dumbravita_host )
        self.addLink( Komarno , Komarno_host )
        self.addLink( Gyor , Gyor_host )
        self.addLink( Sopron , Sopron_host )
        self.addLink( Odesa , Odesa_host )
        self.addLink( Kharkiv , Kharkiv_host )
        self.addLink( Dnipropetrovsk , Dnipropetrovsk_host )
        self.addLink( Lviv , Lviv_host )
        self.addLink( Tarnow , Tarnow_host )
        self.addLink( BanskaBystrica , BanskaBystrica_host )
        self.addLink( NoveZamky , NoveZamky_host )
        self.addLink( Praha , Praha_host )
        self.addLink( Szolnok , Szolnok_host )
        self.addLink( Zalaegerszeg , Zalaegerszeg_host )
        self.addLink( Tapolca , Tapolca_host )
        self.addLink( Tekirdag , Tekirdag_host )
        self.addLink( Cheb , Cheb_host )
        self.addLink( UstinadLabem , UstinadLabem_host )
        self.addLink( HradecKralove , HradecKralove_host )
        self.addLink( Dunaujvaros , Dunaujvaros_host )
        self.addLink( Kiskunhalas , Kiskunhalas_host )
        self.addLink( Tompa , Tompa_host )
        self.addLink( Szeged , Szeged_host )
        self.addLink( Szentes , Szentes_host )
        self.addLink( Zilina , Zilina_host )
        self.addLink( Kosice , Kosice_host )
        self.addLink( Kecskemet , Kecskemet_host )
        self.addLink( Paks , Paks_host )
        self.addLink( Burgas , Burgas_host )
        self.addLink( Debrecen , Debrecen_host )
        self.addLink( Bors , Bors_host )
        self.addLink( Haskovo , Haskovo_host )
        self.addLink( Wroclaw , Wroclaw_host )
        self.addLink( Leszno , Leszno_host )
        self.addLink( Inowroclaw , Inowroclaw_host )
        self.addLink( Poznan , Poznan_host )
        self.addLink( Tczew , Tczew_host )
        self.addLink( Bydgoszcz , Bydgoszcz_host )
        self.addLink( Wloclawek , Wloclawek_host )
        self.addLink( Olsztyn , Olsztyn_host )
        self.addLink( Konin , Konin_host )
        self.addLink( Torun , Torun_host )
        self.addLink( Lublin , Lublin_host )
        self.addLink( Rzeszow , Rzeszow_host )
        self.addLink( Kutno , Kutno_host )
        self.addLink( Bialystok , Bialystok_host )
        self.addLink( Radom , Radom_host )
        self.addLink( Lodz , Lodz_host )
        self.addLink( Krakow , Krakow_host )
        self.addLink( Kielce , Kielce_host )
        self.addLink( Opole , Opole_host )
        self.addLink( Katowice , Katowice_host )
        self.addLink( Svilengrad , Svilengrad_host )
        self.addLink( Edirne , Edirne_host )
        self.addLink( Olomouc , Olomouc_host )
        self.addLink( Hodonin , Hodonin_host )
        self.addLink( Zlin , Zlin_host )
        self.addLink( Ostrava , Ostrava_host )
        self.addLink( Varna , Varna_host )
        self.addLink( CeskeBudejovice , CeskeBudejovice_host )
        self.addLink( Jihlava , Jihlava_host )
        self.addLink( Brno , Brno_host )
        self.addLink( Bielsko , Bielsko_host )
        self.addLink( Most , Most_host )
        self.addLink( Ljubljana , Ljubljana_host )
        self.addLink( Zagreb , Zagreb_host )
        self.addLink( FrankfurtamMain , FrankfurtamMain_host )
        self.addLink( Moskva , Moskva_host )
        self.addLink( SanktPeterburg , SanktPeterburg_host )
        self.addLink( Stockholm , Stockholm_host )
        self.addLink( Budapest , Budapest_host )
        self.addLink( Bratislava , Bratislava_host )
        self.addLink( Wien , Wien_host )
        self.addLink( Berlin , Berlin_host )
        self.addLink( Pristina , Pristina_host )
        self.addLink( Podgorica , Podgorica_host )
        self.addLink( Balikesir , Balikesir_host )
        self.addLink( Izmir , Izmir_host )
        self.addLink( Thessaloniki , Thessaloniki_host )
        self.addLink( Petrich , Petrich_host )
        self.addLink( Pleven , Pleven_host )
        self.addLink( Plovdiv , Plovdiv_host )
        self.addLink( Sarajevo , Sarajevo_host )
        self.addLink( Bucuresti , Bucuresti_host )
        self.addLink( Chisinau , Chisinau_host )
        self.addLink( Sofia , Sofia_host )
        self.addLink( Istanbul , Istanbul_host )
        self.addLink( Istanbul , Istanbul_host )
        self.addLink( Ankara , Ankara_host )
        self.addLink( Skopje , Skopje_host )

        # add edges between routers
        self.addLink( Kobenhavn , Stockholm)
        self.addLink( Kobenhavn , Hamburg)
        self.addLink( Hamburg , FrankfurtamMain)
        self.addLink( Gdansk , Gdynia)
        self.addLink( Gdynia , Slupsk)
        self.addLink( Slupsk , Koszalin)
        self.addLink( Koszalin , Kolobrzeg)
        self.addLink( Kolobrzeg , Szczecin)
        self.addLink( Braniewo , Kaliningrad)
        self.addLink( Poti_Georgia , Varna)
        self.addLink( Georgia_Armenia_Iran_Iraq_Syria , Ankara)
        self.addLink( Amsterdam_London , Wien)
        self.addLink( Amsterdam_London , FrankfurtamMain)
        self.addLink( Lowicz , Warszawa)
        self.addLink( Lowicz , Kutno)
        self.addLink( Ulmi , Dumbravita)
        self.addLink( Ulmi , Bucuresti)
        self.addLink( Beograd , Sofia)
        self.addLink( Beograd , Pristina)
        self.addLink( Beograd , Podgorica)
        self.addLink( Beograd , Sarajevo)
        self.addLink( Beograd , Tompa)
        self.addLink( Beograd , Szeged)
        self.addLink( Balvanesti , Caransebes)
        self.addLink( Balvanesti , Craiova)
        self.addLink( Alexandria , Bucuresti)
        self.addLink( Alexandria , Craiova)
        self.addLink( Arad , Szeged)
        self.addLink( Arad , Timisoara)
        self.addLink( Arad , Oradea)
        self.addLink( Oradea , Bors)
        self.addLink( Oradea , Poieni)
        self.addLink( Caransebes , Timisoara)
        self.addLink( Timisoara , Sofia)
        self.addLink( Ploiesti , Brasov)
        self.addLink( Ploiesti , Bucuresti)
        self.addLink( Giurgiu , Sliven)
        self.addLink( Giurgiu , Pleven)
        self.addLink( Giurgiu , Bucuresti)
        self.addLink( Kazakhstan , Moskva)
        self.addLink( Szombathely , Zalaegerszeg)
        self.addLink( Szombathely , Sopron)
        self.addLink( Szekesfehervar , Budapest)
        self.addLink( Szekesfehervar , Veszprem)
        self.addLink( Tatabanya , Budapest)
        self.addLink( Tatabanya , Gyor)
        self.addLink( Keszthely , Zalaegerszeg)
        self.addLink( Keszthely , Tapolca)
        self.addLink( Veszprem , Tapolca)
        self.addLink( Rozvadov , Cheb)
        self.addLink( Rozvadov , Plzen)
        self.addLink( KarlovyVary , Most)
        self.addLink( KarlovyVary , Cheb)
        self.addLink( Liberec , UstinadLabem)
        self.addLink( Liberec , HradecKralove)
        self.addLink( Plzen , Praha)
        self.addLink( Kyiv , Kharkiv)
        self.addLink( Kyiv , Dnipropetrovsk)
        self.addLink( Kyiv , Lviv)
        self.addLink( Kyiv , Odesa)
        self.addLink( Warszawa , Olsztyn)
        self.addLink( Warszawa , Lublin)
        self.addLink( Warszawa , Bialystok)
        self.addLink( Warszawa , Radom)
        self.addLink( Warszawa , Lodz)
        self.addLink( Warszawa , Vilnius)
        self.addLink( Warszawa , Minsk)
        self.addLink( Tirane , Athina)
        self.addLink( Tirane , Skopje)
        self.addLink( Athina , Thessaloniki)
        self.addLink( Vilnius , Riga)
        self.addLink( Vilnius , Tallinn)
        self.addLink( Tallinn , Riga)
        self.addLink( Tallinn , Stockholm)
        self.addLink( Helsinki , Stockholm)
        self.addLink( Helsinki , SanktPeterburg)
        self.addLink( Poieni , Cluj)
        self.addLink( Sliven , Burgas)
        self.addLink( Sliven , Plovdiv)
        self.addLink( Sliven , Varna)
        self.addLink( Iasi , Bacau)
        self.addLink( Iasi , Galati)
        self.addLink( Iasi , Chisinau)
        self.addLink( Galati , Constanta)
        self.addLink( Brasov , Bacau)
        self.addLink( Medias , Cluj)
        self.addLink( Medias , Dumbravita)
        self.addLink( Constanta , Bucuresti)
        self.addLink( Constanta , Varna)
        self.addLink( Komarno , Bratislava)
        self.addLink( Gyor , Bratislava)
        self.addLink( Gyor , Sopron)
        self.addLink( Sopron , Wien)
        self.addLink( Kharkiv , Dnipropetrovsk)
        self.addLink( Kharkiv , Moskva)
        self.addLink( Lviv , Rzeszow)
        self.addLink( Tarnow , Krakow)
        self.addLink( Tarnow , Rzeszow)
        self.addLink( BanskaBystrica , Bratislava)
        self.addLink( BanskaBystrica , Zilina)
        self.addLink( NoveZamky , Bratislava)
        self.addLink( Praha , Brno)
        self.addLink( Praha , FrankfurtamMain)
        self.addLink( Praha , HradecKralove)
        self.addLink( Praha , CeskeBudejovice)
        self.addLink( Szolnok , Debrecen)
        self.addLink( Szolnok , Budapest)
        self.addLink( Zalaegerszeg , Ljubljana)
        self.addLink( Tekirdag , Edirne)
        self.addLink( Tekirdag , Istanbul)
        self.addLink( UstinadLabem , Most)
        self.addLink( HradecKralove , Ostrava)
        self.addLink( Dunaujvaros , Budapest)
        self.addLink( Dunaujvaros , Kecskemet)
        self.addLink( Kiskunhalas , Tompa)
        self.addLink( Kiskunhalas , Szeged)
        self.addLink( Kiskunhalas , Paks)
        self.addLink( Szeged , Szentes)
        self.addLink( Szentes , Kecskemet)
        self.addLink( Zilina , Kosice)
        self.addLink( Zilina , Bratislava)
        self.addLink( Kosice , Bratislava)
        self.addLink( Paks , Budapest)
        self.addLink( Burgas , Edirne)
        self.addLink( Burgas , Istanbul)
        self.addLink( Burgas , Varna)
        self.addLink( Debrecen , Bors)
        self.addLink( Haskovo , Plovdiv)
        self.addLink( Haskovo , Svilengrad)
        self.addLink( Wroclaw , Leszno)
        self.addLink( Wroclaw , Opole)
        self.addLink( Leszno , Poznan)
        self.addLink( Inowroclaw , Bydgoszcz)
        self.addLink( Inowroclaw , Torun)
        self.addLink( Inowroclaw , Poznan)
        self.addLink( Poznan , Konin)
        self.addLink( Tczew , Bydgoszcz)
        self.addLink( Wloclawek , Torun)
        self.addLink( Wloclawek , Kutno)
        self.addLink( Konin , Kutno)
        self.addLink( Lublin , Rzeszow)
        self.addLink( Kutno , Lodz)
        self.addLink( Radom , Kielce)
        self.addLink( Krakow , Kielce)
        self.addLink( Krakow , Katowice)
        self.addLink( Opole , Katowice)
        self.addLink( Katowice , Bielsko)
        self.addLink( Svilengrad , Edirne)
        self.addLink( Edirne , Istanbul)
        self.addLink( Edirne , Balikesir)
        self.addLink( Olomouc , Brno)
        self.addLink( Olomouc , Ostrava)
        self.addLink( Hodonin , Brno)
        self.addLink( Hodonin , Bratislava)
        self.addLink( Hodonin , Zlin)
        self.addLink( Zlin , Ostrava)
        self.addLink( Ostrava , Bielsko)
        self.addLink( CeskeBudejovice , Jihlava)
        self.addLink( Jihlava , Brno)
        self.addLink( Brno , Bratislava)
        self.addLink( Ljubljana , Wien)
        self.addLink( Ljubljana , Zagreb)
        self.addLink( Zagreb , Wien)
        self.addLink( Zagreb , Sarajevo)
        self.addLink( FrankfurtamMain , Berlin)
        self.addLink( FrankfurtamMain , Wien)
        self.addLink( Moskva , SanktPeterburg)
        self.addLink( Bratislava , Wien)
        self.addLink( Pristina , Skopje)
        self.addLink( Pristina , Sarajevo)
        self.addLink( Podgorica , Sarajevo)
        self.addLink( Balikesir , Izmir)
        self.addLink( Balikesir , Istanbul)
        self.addLink( Izmir , Ankara)
        self.addLink( Thessaloniki , Petrich)
        self.addLink( Petrich , Plovdiv)
        self.addLink( Pleven , Sofia)
        self.addLink( Plovdiv , Sofia)
        self.addLink( Sofia , Skopje)
        self.addLink( Istanbul , Istanbul)
        self.addLink( Istanbul , Ankara)

	super(SimpleOSPFNet, self).build(*args, **kwargs)

    def addRouter_v4(self, name):
        return self.addRouter(name, use_v4=True, use_v6=False)


ipmininet.DEBUG_FLAG = True
lg.setLogLevel("info")

# Start network
net = IPNet(topo=SimpleOSPFNet(), use_v4=True, use_v6=False, allocate_IPs=True, ipBase=u'192.0.0.0/8')
net.start()
IPCLI(net)
net.stop()
