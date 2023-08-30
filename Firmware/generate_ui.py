Import("env")

# You must have nodejs/npm installed
env.Execute(f"cd ui-ng && npm install --legacy-peer-deps")
env.Execute(f"cd ui-ng && npm run build-silent")
