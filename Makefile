.PHONY: up down view orders

# Start core infra (exchange + recorder)
up:
	docker compose up

# Stop everything and remove volumes
down:
	docker compose down -v

# Run the terminal UI (in a container)
view:
	docker compose run --rm marketview

# Run the local order simulator script
orders:
	bash ./sim_orders.sh
