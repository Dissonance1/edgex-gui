import requests
import yaml
from datetime import datetime

TOKEN_API_URL = "https://rapid.meridiandatalabs.com/vault/v1/identity/oidc/token/mdl_admin4"
AUTH_TOKEN = "<YOUR_VAULT_TOKEN>"

CONFIG_FILE = "/home/amit/src/edgex/app-functions-sdk-go/app-service-template/res/configuration.yaml"


def load_config():
    with open(CONFIG_FILE, "r") as f:
        return yaml.safe_load(f)


def save_config(config):
    with open(CONFIG_FILE, "w") as f:
        yaml.safe_dump(config, f)


def should_refresh(config):
    """Check last refresh date inside YAML (not using tmp file)."""

    today = datetime.now().strftime("%Y-%m-%d")

    last_date = config["ApplicationSettings"].get("LastTokenRefreshDate", None)

    if last_date is None:
        return True     # First-time install → refresh now

    return last_date != today  # If date changed → refresh


def update_token():
    print("⏳ Calling API to get new token...")

    headers = {"Authorization": f"Bearer {AUTH_TOKEN}"}
    response = requests.get(TOKEN_API_URL, headers=headers)

    response.raise_for_status()
    body = response.json()

    new_token = body["data"]["token"]
    bearer_value = f"Bearer {new_token}"

    print("🔑 New token received.")

    # Load config again
    config = load_config()

    # Update token
    config["Writable"]["InsecureSecrets"]["rapid"]["SecretData"]["headervalue"] = bearer_value
    config["ApplicationSettings"]["RapidAuthHeader"] = bearer_value

    # Update date inside YAML
    today = datetime.now().strftime("%Y-%m-%d")
    config["ApplicationSettings"]["LastTokenRefreshDate"] = today

    save_config(config)

    print("✅ Token refreshed & YAML updated.")
    print(f"➡ Updated Token: {bearer_value}")



def main():
    config = load_config()

    if should_refresh(config):
        print("🔄 New day or first run — refreshing token now.")
        update_token()
    else:
        print("⛔ Token already refreshed today — skipping.")


if __name__ == "__main__":
    main()
