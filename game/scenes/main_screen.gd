extends Control

const PHASE_DURATION := 180.0 # 3 minutes
const SELL_INTERVAL := 1.0 # sale attempt every second

@onready var game_state = $GameState

@onready var money_label: Label = $MarginContainer/NinePatchRect/Money/Label

@onready var morning_icon: TextureRect = $MarginContainer/NinePatchRect/Time/Morning
@onready var noon_icon: TextureRect = $MarginContainer/NinePatchRect/Time/Noon
@onready var night_icon: TextureRect = $MarginContainer/NinePatchRect/Time/Night

@onready var decrease_button: Button = $MarginContainer/NinePatchRect/Price/Slider/Decrease
@onready var price_label: Label = $MarginContainer/NinePatchRect/Price/Slider/Label
@onready var increase_button: Button = $MarginContainer/NinePatchRect/Price/Slider/Increase
@onready var sell_chance_label: Label = $MarginContainer/NinePatchRect/Price/Demand/Label2

@onready var change_button: Button = $MarginContainer/NinePatchRect/Feature/NinePatchRect/MarginContainer/Change
@onready var potion_name_label: Label = $MarginContainer/NinePatchRect/Feature/NinePatchRect/MarginContainer/MarginContainer/potionandname/nameofpotion

@onready var tech_tree_button: Button = $"MarginContainer/NinePatchRect/Tech_tree_margin/Tech Tree"
@onready var mixing_button: Button = $MarginContainer/NinePatchRect/Mixing_margin/Mixing

var phase_timer := 0.0
var sell_timer := 0.0

func _ready() -> void:
	decrease_button.pressed.connect(_on_decrease_pressed)
	increase_button.pressed.connect(_on_increase_pressed)
	mixing_button.pressed.connect(_on_mixing_pressed)
	tech_tree_button.pressed.connect(_on_tech_tree_pressed)
	change_button.pressed.connect(_on_mixing_pressed)

	_update_ui()
	_update_time_icons()

func _process(delta: float) -> void:
	phase_timer += delta
	sell_timer += delta

	if sell_timer >= SELL_INTERVAL:
		sell_timer = 0.0
		game_state.attempt_sale()
		_update_ui()

	if phase_timer >= PHASE_DURATION:
		phase_timer = 0.0
		game_state.advance_phase()
		_update_ui()
		_update_time_icons()

func _on_decrease_pressed() -> void:
	game_state.set_price(game_state.get_price() - 1)
	_update_ui()

func _on_increase_pressed() -> void:
	game_state.set_price(game_state.get_price() + 1)
	_update_ui()

func _update_ui() -> void:
	money_label.text = str(game_state.get_gold())
	price_label.text = str(int(game_state.get_price()))
	sell_chance_label.text = str(game_state.get_sell_chance_percent()) + "% PER SECOND"
	potion_name_label.text = game_state.get_featured_potion_name()

func _update_time_icons() -> void:
	var phase : String = game_state.get_current_phase()

	morning_icon.visible = phase == "sun"
	noon_icon.visible = phase == "noon"
	night_icon.visible = phase == "night"

func _on_mixing_pressed() -> void:
	get_tree().change_scene_to_file("res://scenes/mixing_screen.tscn")

func _on_tech_tree_pressed() -> void:
	get_tree().change_scene_to_file("res://scenes/tech_tree.tscn")
