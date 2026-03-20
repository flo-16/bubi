# Workflow
### Allgemein
- Das remote-Repository dient nur als BackUp, es wird nicht gepullt.
- Der main-Branch ist der Release-Branch.
- Der dev-Branch ist der Entwickler-Branch.
- Hierzu kurzlebige Branches erstellen, die dann in den dev-Branch gemergt werden.
### Entwicklung
- git checkout -b _neuer-branch-name_ dev
- alle Änderungen durchführen
- git add __File__
- git commit -m "_Commit-Nachricht_"
- git switch dev
- git merge --no-ff _neuer-branch-name_
- git branch -d _neuer-branch-name_
### Release
- git switch main
- git merge dev
- git tag -a _vx.x_ -m "_Tag-Nachricht_"
- git push
- git push --tags
#### Diesen Workflow unbedingt einhalten!
