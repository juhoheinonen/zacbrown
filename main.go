package main

import (
	"fmt"

	rl "github.com/gen2brain/raylib-go/raylib"
)

// Constants
const (
	MaxFrameSpeed    = 15
	MinFrameSpeed    = 1
	GameMapWidth     = 80
	GameMapHeight    = 60
	TileSize         = 8
	GameScreenWidth  = 640
	GameScreenHeight = 480
)

// TileType represents the type of tile
type TileType int

const (
	BrownGround TileType = iota
	LightSky
)

// GameTile represents a single tile in the game map
type GameTile struct {
	X        int
	Y        int
	TileType TileType
}

// Hitbox represents the collision bounds of an entity
type Hitbox struct {
	LeftX   int
	RightX  int
	TopY    int
	BottomY int
}

// MainCharacter represents the player character
type MainCharacter struct {
	Position        rl.Vector2
	Hitbox          Hitbox
	HeightPixels    int
	HorizontalSpeed int
	VerticalSpeed   int
	JumpingPower    int
}

// initializeGameMap creates the game map with ground at the bottom
func initializeGameMap() [GameMapWidth][GameMapHeight]GameTile {
	var gameMap [GameMapWidth][GameMapHeight]GameTile

	for x := 0; x < GameMapWidth; x++ {
		for y := 0; y < GameMapHeight; y++ {
			if y >= GameMapHeight-4 {
				gameMap[x][y] = GameTile{X: x, Y: y, TileType: BrownGround}
			} else {
				gameMap[x][y] = GameTile{X: x, Y: y, TileType: LightSky}
			}
		}
	}

	return gameMap
}

// drawMapTiles renders all tiles in the game map
func drawMapTiles(gameMap *[GameMapWidth][GameMapHeight]GameTile, brownGroundTexture, lightSkyTexture rl.Texture2D) {
	for x := 0; x < GameMapWidth; x++ {
		for y := 0; y < GameMapHeight; y++ {
			var tileTexture rl.Texture2D
			if gameMap[x][y].TileType == BrownGround {
				tileTexture = brownGroundTexture
			} else {
				tileTexture = lightSkyTexture
			}

			tilePosition := rl.Vector2{X: float32(x * TileSize), Y: float32(y * TileSize)}
			textureRectangle := rl.Rectangle{
				X:      float32(x * TileSize),
				Y:      float32(y * TileSize),
				Width:  float32(tileTexture.Width),
				Height: float32(tileTexture.Height),
			}
			rl.DrawTextureRec(tileTexture, textureRectangle, tilePosition, rl.White)
		}
	}
}

// checkGroundCollision checks if player is on ground and updates vertical speed
func checkGroundCollision(player *MainCharacter, gameMap *[GameMapWidth][GameMapHeight]GameTile) {
	for i := int(player.Position.X) + player.Hitbox.LeftX; i < int(player.Position.X)+player.Hitbox.RightX; i++ {
		yDividedByTileSize := (int(player.Position.Y) + player.HeightPixels) / TileSize
		xCalculated := int(player.Position.X) * TileSize / GameMapWidth

		if xCalculated >= 0 && xCalculated < GameMapWidth && yDividedByTileSize >= 0 && yDividedByTileSize < GameMapHeight {
			belowTileBlocking := gameMap[xCalculated][yDividedByTileSize].TileType == BrownGround

			if belowTileBlocking {
				player.VerticalSpeed = 0
				break
			} else {
				player.VerticalSpeed = 5
			}
		}
	}
}

// isOnGround checks if the player is currently on the ground
func isOnGround(player *MainCharacter, gameMap *[GameMapWidth][GameMapHeight]GameTile) bool {
	for i := int(player.Position.X) + player.Hitbox.LeftX; i < int(player.Position.X)+player.Hitbox.RightX; i++ {
		yDividedByTileSize := (int(player.Position.Y) + player.HeightPixels) / TileSize
		xCalculated := int(player.Position.X) * TileSize / GameMapWidth

		if xCalculated >= 0 && xCalculated < GameMapWidth && yDividedByTileSize >= 0 && yDividedByTileSize < GameMapHeight {
			if gameMap[xCalculated][yDividedByTileSize].TileType == BrownGround {
				return true
			}
		}
	}

	return false
}

func min(a, b float32) float32 {
	if a < b {
		return a
	}
	return b
}

func main() {
	// Enable config flags for resizable window and vertical synchro
	rl.SetConfigFlags(rl.FlagWindowResizable | rl.FlagVsyncHint)
	rl.InitWindow(int32(rl.GetScreenWidth()), int32(rl.GetScreenHeight()), "Zachary Brown")
	rl.SetWindowMinSize(320, 240)

	// Render texture initialization, used to hold the rendering result so we can easily resize it
	target := rl.LoadRenderTexture(GameScreenWidth, GameScreenHeight)
	rl.SetTextureFilter(target.Texture, rl.FilterBilinear)

	// Load textures
	brownieStanding := rl.LoadTexture("img/brownie.png")
	brownieRunning := rl.LoadTexture("img/brownie_running.png")
	brownGroundTexture := rl.LoadTexture("img/brown_ground.png")
	lightSkyTexture := rl.LoadTexture("img/light_sky.png")

	frameRec := rl.Rectangle{
		X:      0,
		Y:      0,
		Width:  float32(brownieRunning.Width) / 6,
		Height: float32(brownieRunning.Height),
	}
	currentFrame := 0

	// Initialize map
	gameMap := initializeGameMap()

	// Initialize player character
	playerCharacter := MainCharacter{
		Position:        rl.Vector2{X: 1.0, Y: 300.0},
		HorizontalSpeed: 0,
		VerticalSpeed:   0,
		HeightPixels:    64,
		JumpingPower:    0,
		Hitbox:          Hitbox{LeftX: 16, RightX: 50, TopY: 0, BottomY: 60},
	}

	framesCounter := 0
	framesSpeed := 10 // Number of spritesheet frames shown by second

	rl.SetTargetFPS(60)

	// Main game loop
	for !rl.WindowShouldClose() {
		scale := min(float32(rl.GetScreenWidth())/GameScreenWidth, float32(rl.GetScreenHeight())/GameScreenHeight)

		// Input handling
		if rl.IsKeyDown(rl.KeyRight) {
			playerCharacter.HorizontalSpeed = 5
		} else if rl.IsKeyDown(rl.KeyLeft) {
			playerCharacter.HorizontalSpeed = -5
		} else {
			playerCharacter.HorizontalSpeed = 0
		}

		// Jump
		if rl.IsKeyPressed(rl.KeyRightControl) {
			if isOnGround(&playerCharacter, &gameMap) {
				playerCharacter.JumpingPower = 20
				playerCharacter.VerticalSpeed = -5
			}
		}

		if playerCharacter.JumpingPower <= 0 {
			checkGroundCollision(&playerCharacter, &gameMap)
		}

		// Update debug information
		yDividedByTileSize := (int(playerCharacter.Position.Y) + playerCharacter.HeightPixels) / TileSize
		yModuloByTileSize := (int(playerCharacter.Position.Y) + playerCharacter.HeightPixels) % TileSize
		fallingInformation := fmt.Sprintf("y/ts: %d, ymod: %d, jump: %d", yDividedByTileSize, yModuloByTileSize, playerCharacter.JumpingPower)

		// Update animation
		framesCounter++
		if framesCounter >= (60 / framesSpeed) {
			framesCounter = 0
			currentFrame++

			if currentFrame > 5 {
				currentFrame = 0
			}

			frameRec.X = float32(currentFrame) * float32(brownieRunning.Width) / 6
		}

		// Draw to render texture
		rl.BeginTextureMode(target)
		rl.ClearBackground(rl.RayWhite)

		// Draw the map tiles
		drawMapTiles(&gameMap, brownGroundTexture, lightSkyTexture)

		// Update player position based on vertical speed
		if playerCharacter.VerticalSpeed > 0 {
			playerCharacter.Position.Y += float32(playerCharacter.VerticalSpeed)
		} else if playerCharacter.VerticalSpeed < 0 {
			if playerCharacter.JumpingPower > 0 {
				playerCharacter.Position.Y += float32(playerCharacter.VerticalSpeed)
				playerCharacter.JumpingPower--
			} else {
				playerCharacter.Position.Y += float32(playerCharacter.VerticalSpeed)
				playerCharacter.VerticalSpeed++
			}
		}

		// Draw player
		if playerCharacter.HorizontalSpeed == 0 {
			staticRec := rl.Rectangle{X: 0, Y: 0, Width: float32(brownieStanding.Width), Height: float32(brownieStanding.Height)}
			rl.DrawTextureRec(brownieStanding, staticRec, playerCharacter.Position, rl.White)
		} else if playerCharacter.HorizontalSpeed > 0 {
			playerCharacter.Position.X += float32(playerCharacter.HorizontalSpeed)
			rl.DrawTextureRec(brownieRunning, frameRec, playerCharacter.Position, rl.White)
		} else {
			playerCharacter.Position.X += float32(playerCharacter.HorizontalSpeed)
			// Flip texture horizontally by using negative width
			flippedFrameRec := rl.Rectangle{
				X:      frameRec.X + frameRec.Width,
				Y:      frameRec.Y,
				Width:  -frameRec.Width,
				Height: frameRec.Height,
			}
			destRec := rl.Rectangle{
				X:      playerCharacter.Position.X,
				Y:      playerCharacter.Position.Y,
				Width:  frameRec.Width,
				Height: frameRec.Height,
			}
			rl.DrawTexturePro(brownieRunning, flippedFrameRec, destRec, rl.Vector2{X: 0, Y: 0}, 0.0, rl.White)
		}

		// Draw debug text
		pcPositionInfo := fmt.Sprintf("x: %.1f, y: %.1f", playerCharacter.Position.X, playerCharacter.Position.Y)
		rl.DrawText(pcPositionInfo, 10, 10, 20, rl.Black)
		rl.DrawText(fallingInformation, 350, 10, 20, rl.Black)

		rl.EndTextureMode()

		// Draw render texture to screen
		rl.BeginDrawing()
		rl.ClearBackground(rl.Black)

		sourceRec := rl.Rectangle{
			X:      0,
			Y:      0,
			Width:  float32(target.Texture.Width),
			Height: -float32(target.Texture.Height),
		}
		destRec := rl.Rectangle{
			X:      (float32(rl.GetScreenWidth()) - GameScreenWidth*scale) * 0.5,
			Y:      (float32(rl.GetScreenHeight()) - GameScreenHeight*scale) * 0.5,
			Width:  GameScreenWidth * scale,
			Height: GameScreenHeight * scale,
		}
		rl.DrawTexturePro(target.Texture, sourceRec, destRec, rl.Vector2{X: 0, Y: 0}, 0.0, rl.White)

		rl.EndDrawing()
	}

	// Cleanup
	rl.UnloadTexture(brownieRunning)
	rl.UnloadTexture(brownieStanding)
	rl.UnloadTexture(brownGroundTexture)
	rl.UnloadTexture(lightSkyTexture)
	rl.UnloadRenderTexture(target)

	rl.CloseWindow()
}
