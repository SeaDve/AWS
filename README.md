# KT-Weather-Station

## Connectivity Flags

This flag determines the connectivity method for data transmission.

| Flag      | Description                                                                                                             |
| --------- | ----------------------------------------------------------------------------------------------------------------------- |
| `USE_GSM` | Enable GSM module support for remote data transmission. Otherwise, data is sent via Wi-Fi, also defining `WIFI_1` flag. |

### APN Flags for GSM Module

This flag determines the APN settings for the GSM module. Only one APN flag should be defined at a time.

| Flag        | Description             |
| ----------- | ----------------------- |
| `SMART_APN` | Use SMART APN settings. |
| `GLOBE_APN` | Use GLOBE APN settings. |
| `GOMO_APN`  | Use GOMO APN settings.  |

### Station Flags

This flag determines the device serial number and the station name.

| Flag                     | Description                   |
| ------------------------ | ----------------------------- |
| `DEMO_MARIA_STATION`     | Demo Station - Maria          |
| `SABANG_STATION`         | Sabang Fish Landing           |
| `BAGAC_STATION`          | BPSU Bagac Campus             |
| `MARIVELES_STATION`      | Mariveles Municipal Hall      |
| `LIMAY_STATION`          | Limay Physical Therapy Center |
| `COMMAND_CENTER_STATION` | One Bataan Command Center     |
| `HERMOSA_STATION`        | Hermosa Municipal Hall        |
| `CABCABEN_STATION`       | Old Cabcaben Pier             |
| `QUINAWAN_STATION`       | Quinawan Integrated School    |
| `KANAWAN_STATION`        | Kanawan Integrated School     |
| `TANATO_STATION`         | Tanato Elementary School      |
| `DINALUPIHAN_STATION`    | BPSU Dinalupihan Campus       |
| `PAGASA_STATION`         | Pag-asa Elementary School     |
| `DEMO_GLENN_STATION`     | Demo Station - Glenn          |
| `PTORIVAS_STATION`       | Pto. Rivas Fish Landing       |
