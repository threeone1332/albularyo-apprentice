extends Control

const SAVE_PATH = "user://savegame.json"

@onready var panel: Panel = $Panel

func _ready() -> void:
	panel.visible = false

func _on_start_button_pressed():
	if FileAccess.file_exists(SAVE_PATH):
		load_game()
		# CHANGED: Route existing save loads through your loading screen first!
		get_tree().change_scene_to_file("res://scenes/game_loading_screen.tscn")
	else:
		# Fresh game goes to the story introduction first
		get_tree().change_scene_to_file("res://scenes/intro_cutscene.tscn")

func _on_settings_pressed():
	panel.visible = true

func load_game(): 
	var file = FileAccess.open(SAVE_PATH, FileAccess.READ)
	if file == null:
		return
	var data = JSON.parse_string(file.get_as_text())
	file.close()
	if data == null:
		return

func _process(_delta: float) -> void:
	pass

func _on_exit_settings_pressed() -> void:
	panel.visible = false

func _on_quit_pressed() -> void:
	get_tree().quit()
