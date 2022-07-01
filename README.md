This is a reincarnation of a nice TotalCommander plugin (by Serge Lamikhov-Center) 
made for DoubleCommander users.

Update 1 Jun 2022: Now both GTK and Qt5!


requirements:

- libgtk2.0-dev (for gtk version)
- qtbase5-dev   (for qt5 version)
- ELFIO (https://github.com/serge1/ELFIO) - downloads automatically


Attention:
Qt5 version of this plugin does not work properly if the lister was started in "Preview" mode (key "0" to off/on).
It doesn't receive PgUp/PgDown/Home/End/etc keyPress events, I dunno why =\
Disable "Preview" mode or just tap "0" twice to reinitialize the lister window.