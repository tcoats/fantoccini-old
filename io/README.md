# Fantoccini IO

# State
For writing save games, configuration or modding per user.

1. OS X: `/Users/tcoats/Library/Application Support/Fantoccini`
2. Windows: `C:\users\tcoats\Application Data\Fantoccini`
3. Linux: `/home/tcoats/.local/share/fantoccini`

# Resources
All resources are loaded from these locations, in order:

1. State directory above
2. Resources directory:
  - OS X: `Fantoccini.app/Contents/Resources`
  - Windows / Linux: `resources`
3. Folders or archive files defined in the `manifest.txt` file relative to the resources directory above.

