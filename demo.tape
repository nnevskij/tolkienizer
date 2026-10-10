# Output destination
Output assets/demo.gif

# Terminal appearance settings
Set Shell "zsh"
Set FontSize 18
Set Width 1100
Set Height 640
Set Padding 25
Set WindowBar Colorful
Set WindowBarSize 36
Set BorderRadius 10
Set Theme "Catppuccin Mocha"
Set PlaybackSpeed 1.0
Set TypingSpeed 40ms

# Setup path and clear terminal (hidden)
Hide
Type "export PATH=$PWD:$PATH"
Enter
Type "clear"
Enter
Show

Sleep 800ms

# Demo 1: Default Epic Transformation
Type 'tolkienizer "The database server crashed due to memory exhaustion"'
Sleep 400ms
Enter

Sleep 5s

Sleep 1.5s

# Demo 2: Cozy Shire / Hobbit Style
Type 'tolkienizer -s hobbit "I have to go buy tea and butter at the grocery store"'
Sleep 400ms
Enter

Sleep 5s

Sleep 1.5s

# Demo 3: Shadow of Mordor via Unix Pipe
Type 'echo "Deploy the release before Friday" | tolkienizer -s mordor'
Sleep 400ms
Enter

Sleep 5s

Sleep 3s
