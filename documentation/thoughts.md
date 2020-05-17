# Thoughts about the project and the engine

Much like a wad file, we could package game content into a zip file.

## File structure

world.fun /
levels/
level1.janet
level2.janet
level3.janet

    scripts/
        anim-trigger.janet

    block.atlas.png
    map.janet

## File details

- levels/<level-name>.janet

  Must return a janet dictionary

  Example:

  ```clojure
  @{ :size @[5 5]
      :start @[10 0 32]
      :layers @[ @{ :data ``#####`` }
                  @{ :data ``#...#`` }
                  @{ :data ``#...#`` }
                  @{ :data ``#...#`` }
                  @{ :data ``#####`` } ]
      :props @[]
      :actors @[] }
  ```

- map.janet

  Contains the chaining of levels

  @{ }
