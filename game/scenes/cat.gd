extends TextureButton

@export var item_name: String = "Cat Companion"
@export var price: int = 200

@onready var shop = get_parent()

func _ready():
	mouse_entered.connect(_on_mouse_entered)
	mouse_exited.connect(_on_mouse_exited)
	pressed.connect(_on_pressed)

func _on_mouse_entered():
	if shop.money >= price:
		shop.message_label.text = "Click to buy " + item_name
	else:
		shop.message_label.text = "Not enough coins"

func _on_mouse_exited():
	shop.message_label.text = ""

func _on_pressed():
	if shop.money >= price:
		shop.money -= price
		shop.update_money_label()

		shop.message_label.text = "Bought " + item_name
	else:
		shop.message_label.text = "You cannot buy this"
