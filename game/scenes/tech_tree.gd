extends Control

var money: int = 400

@onready var money_label = $MoneyLabel
@onready var message_label = $MessageLabel

func _ready():
	update_money_label()

func update_money_label():
	money_label.text = "Coins: " + str(money)
