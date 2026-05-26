# intro_cutscene.gd
extends Node2D

@onready var label: Label = $CanvasLayer/Label
@onready var animation_player: AnimationPlayer = $CanvasLayer/AnimationPlayer

var lines = [
	"Your Master...",
	"...",
	"She has perished.",
	"Her remedies are running low.",
	"The people are still coming.",
	"You are not ready.",
	"But you are all they have."
]

var current_line = 0

func _ready():
	label.modulate.a = 0.0
	show_current_line()

func show_current_line():
	if current_line >= lines.size():
		# Route to the Albularyo loading sequence
		get_tree().change_scene_to_file("res://scenes/game_loading_screen.tscn")
		return
	label.text = lines[current_line]
	animation_player.play("fade_in_out")

func _on_animation_player_animation_finished(_anim_name: String):
	current_line += 1
	show_current_line()
