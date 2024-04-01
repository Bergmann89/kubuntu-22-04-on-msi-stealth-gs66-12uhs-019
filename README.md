# Install Kubuntu 22.04 on MSI Stealth GS66 12UHS-091

- Install Kubuntu as usual
- Adapt UI Scale
    - Set the UI scale in the display configuration
    - Set cursor size (to 48)
    - Set PLASMA_USE_QT_SCALING=1 in your profile
        https://www.reddit.com/r/kde/comments/o8bb4s/inconsistent_icon_sizes_of_small_icons_across_kde/
    - Apply plasma settings to the login screen (see SDDM menu)
- Get keyboard backlight to work
    - Install dependencies `apt-get install python3 python3-pip libhidapi-dev`
    - Get the msi-perkeyrgb project https://github.com/Bergmann89/msi-perkeyrgb (or the original one once it is patched https://github.com/Askannz/msi-perkeyrgb)
- Setup fan control
    - Install msi-ec kernel module: https://github.com/Bergmann89/msi-ec
    - Install the `set-fan-control`
        ```
        $ cp ./set-fan-control /usr/local/bin/set-fan-control
        $ cp ./set-fan-control.service /lib/systemd/system/set-fan-control.service
        $ systemctl enable set-fan-control.service 
        ```
