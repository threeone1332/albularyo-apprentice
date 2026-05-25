extends Control

@onready var panel: Panel = $Panel
@onready var main_menu: Control = $"."

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.
	panel.visible = false;

func on_settings_pressed():
	panel.visible = true;

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
