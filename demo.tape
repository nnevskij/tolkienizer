# Output destination
Output assets/demo.gif

# Terminal appearance settings
Set Shell "zsh"
Set FontSize 20
Set Width 1200
Set Height 700
Set Padding 30
Set WindowBar Colorful
Set WindowBarSize 36
Set BorderRadius 10
Set Theme "Catppuccin Mocha"
Set PlaybackSpeed 1.0
Set TypingSpeed 35ms

# Setup path, demo mode, and clear terminal (hidden)
Hide
Type "export PATH=$PWD:$PATH"
Enter
Type "export TOLKIENIZER_DEMO=1"
Enter
Type "clear"
Enter
Show

Sleep 1s

# Demo 1: Default Epic Transformation
Type 'tolkienizer "The database server crashed due to memory exhaustion"'
Sleep 300ms
Enter

# Allow spinner to finish and display output
Sleep 2s
Sleep 4s

# Demo 2: Cozy Shire / Hobbit Style
Type 'tolkienizer -s hobbit "I have to buy tea and butter at the grocery store"'
Sleep 300ms
Enter

Sleep 2s
Sleep 4s

# Demo 3: Shadow of Mordor via Unix Pipe
Type 'echo "Deploy the release before Friday" | tolkienizer -s mordor'
Sleep 300ms
Enter

Sleep 2s
Sleep 4.5s
