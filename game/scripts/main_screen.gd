extends Control

const PHASE_DURATION := 15.0
const SELL_INTERVAL := 1.0

@onready var game_state = get_node("/root/GlobalGameState")

@onready var money_label: Label = $MarginContainer/NinePatchRect/MoneyArea/Money/Label
@onready var gain_label: Label = $MarginContainer/NinePatchRect/MoneyArea/GainLabel
@onready var sale_feedback: Label = $SaleFeedback

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

@onready var potion_icon: NinePatchRect = $MarginContainer/NinePatchRect/Feature/NinePatchRect/MarginContainer/MarginContainer/potionandname/NinePatchRect/MarginContainer/picofpotion

var phase_timer := 0.0
var sell_timer := 0.0
var feedback_id := 0
var gain_id := 0

var potion_icon_regions := [
	Rect2(20, 20, 200, 200), # Lunas ng Sigla
	Rect2(260, 20, 200, 195),    # Lunas ng Lihim
	Rect2(500, 20, 200, 195),    # Lunas ng Linaw
	Rect2(740, 20, 150, 146),    # Lunas ng Himbing
	Rect2(930, 20, 200, 200),    # Tinctura ng Bilis
	Rect2(1170, 20, 200, 200),    # Lason
	Rect2(1410, 20, 200, 200)     # Lunas ng Diwa
]

func _ready() -> void:
	sale_feedback.visible = false
	gain_label.text = ""
	gain_label.visible = true

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
		_attempt_sale_tick()

	if phase_timer >= PHASE_DURATION:
		phase_timer = 0.0
		_advance_phase_tick()

func _attempt_sale_tick() -> void:
	if not game_state.has_featured_stock():
		_show_sale_feedback("NO POTION", false)
	else:
		var sold: bool = game_state.attempt_sale()

		if sold:
			var gained := int(game_state.get_price())
			_show_sale_feedback("SOLD!", true)
			_show_gold_gain(gained)
		else:
			_show_sale_feedback("NO SALE", false)

	_update_ui()

func _advance_phase_tick() -> void:
	var old_gold := int(game_state.get_gold())

	game_state.advance_phase()

	var new_gold := int(game_state.get_gold())
	var difference := new_gold - old_gold

	if difference < 0:
		_show_sale_feedback("ADVENTURERS PAID", false)
		_show_gold_loss(abs(difference))

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
	var potion_id: int = game_state.get_featured_potion_id()
	potion_icon.region_rect = potion_icon_regions[potion_id]

func _update_time_icons() -> void:
	var phase: String = game_state.get_current_phase()

	morning_icon.visible = phase == "sun"
	noon_icon.visible = phase == "noon"
	night_icon.visible = phase == "night"

func _show_sale_feedback(text: String, sold: bool) -> void:
	feedback_id += 1
	var current_id := feedback_id

	sale_feedback.text = text
	sale_feedback.visible = true
	sale_feedback.modulate.a = 1.0

	if sold:
		sale_feedback.add_theme_color_override("font_color", Color.html("#A97839"))
	else:
		sale_feedback.add_theme_color_override("font_color", Color.html("#8A5A52"))

	var start_pos := sale_feedback.position
	var end_pos := start_pos + Vector2(0, -18)

	var tween := create_tween()
	tween.tween_interval(0.25)
	tween.set_parallel(true)
	tween.tween_property(sale_feedback, "position", end_pos, 0.9)
	tween.tween_property(sale_feedback, "modulate:a", 0.0, 0.9)

	await tween.finished

	if current_id == feedback_id:
		sale_feedback.visible = false
		sale_feedback.position = start_pos
		sale_feedback.modulate.a = 1.0

func _show_gold_gain(amount: int) -> void:
	gain_id += 1
	var current_id := gain_id

	gain_label.text = "+" + str(amount)
	gain_label.add_theme_color_override("font_color", Color.html("#D39B38"))
	gain_label.modulate.a = 1.0
	gain_label.scale = Vector2(1.15, 1.15)

	var start_pos := gain_label.position
	var end_pos := start_pos + Vector2(18, -35)

	var tween := create_tween()
	tween.tween_interval(0.15)
	tween.set_parallel(true)
	tween.tween_property(gain_label, "position", end_pos, 1.0)
	tween.tween_property(gain_label, "modulate:a", 0.0, 1.0)
	tween.tween_property(gain_label, "scale", Vector2(1.35, 1.35), 0.25)

	await tween.finished

	if current_id == gain_id:
		gain_label.text = ""
		gain_label.position = start_pos
		gain_label.modulate.a = 1.0
		gain_label.scale = Vector2.ONE

func _show_gold_loss(amount: int) -> void:
	gain_id += 1
	var current_id := gain_id

	gain_label.text = "-" + str(amount)
	gain_label.add_theme_color_override("font_color", Color.html("#8A5A52"))
	gain_label.modulate.a = 1.0
	gain_label.scale = Vector2(1.5, 1.5)

	var start_pos := gain_label.position
	var end_pos := start_pos + Vector2(18, -35)

	var tween := create_tween()
	tween.tween_interval(0.15)
	tween.set_parallel(true)
	tween.tween_property(gain_label, "position", end_pos, 1.0)
	tween.tween_property(gain_label, "modulate:a", 0.0, 1.0)
	tween.tween_property(gain_label, "scale", Vector2(2.0, 2.0), 0.22)

	await tween.finished

	if current_id == gain_id:
		gain_label.text = ""
		gain_label.position = start_pos
		gain_label.modulate.a = 1.0
		gain_label.scale = Vector2.ONE

func _on_mixing_pressed() -> void:
	get_tree().change_scene_to_file("res://scenes/mixing_screen.tscn")

func _on_tech_tree_pressed() -> void:
	get_tree().change_scene_to_file("res://scenes/tech_tree.tscn")
