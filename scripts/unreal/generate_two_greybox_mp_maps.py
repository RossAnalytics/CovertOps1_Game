import unreal

EDITOR_ASSET_LIBRARY = unreal.EditorAssetLibrary
EDITOR_LEVEL_LIBRARY = unreal.EditorLevelLibrary
LEVEL_UTILS = unreal.EditorLoadingAndSavingUtils

MESH_CUBE = "/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube"
MESH_CHAMFER = "/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube"
MESH_CYLINDER = "/Game/LevelPrototyping/Meshes/SM_Cylinder.SM_Cylinder"
MESH_RAMP = "/Game/LevelPrototyping/Meshes/SM_Ramp.SM_Ramp"

MAPS_FOLDER = "/Game/Multiplayer/Maps"

FLAG_BP = "/Game/Multiplayer/Objectives/BP_COFlagObjectiveActor.BP_COFlagObjectiveActor"
BOMB_BP = "/Game/Multiplayer/Objectives/BP_COBombObjectiveActor.BP_COBombObjectiveActor"
SITE_BP = "/Game/Multiplayer/Objectives/BP_COBombSiteActor.BP_COBombSiteActor"
NEON_VISUAL_DIRECTOR_BP = "/Game/Multiplayer/Visual/BP_CONeonDistrictVisualDirector.BP_CONeonDistrictVisualDirector"



def log(message: str) -> None:
    unreal.log(f"[CovertOps1MapGen] {message}")



def warn(message: str) -> None:
    unreal.log_warning(f"[CovertOps1MapGen] {message}")



def ensure_directory(path: str) -> None:
    if not EDITOR_ASSET_LIBRARY.does_directory_exist(path):
        EDITOR_ASSET_LIBRARY.make_directory(path)
        log(f"Created directory: {path}")



def load_asset(path: str):
    asset = unreal.load_asset(path)
    if not asset:
        warn(f"Missing asset: {path}")
    return asset



def get_blueprint_generated_class(blueprint_path: str):
    blueprint = load_asset(blueprint_path)
    if not blueprint:
        return None

    generated_class = getattr(blueprint, "generated_class", None)
    if callable(generated_class):
        generated_class = generated_class()

    if not generated_class:
        warn(f"Could not resolve generated class for {blueprint_path}")
    return generated_class



def spawn_actor(actor_class, location, rotation=unreal.Rotator(0.0, 0.0, 0.0)):
    return EDITOR_LEVEL_LIBRARY.spawn_actor_from_class(actor_class, location, rotation)



def spawn_mesh(mesh_path: str, location, scale, rotation=unreal.Rotator(0.0, 0.0, 0.0), label: str = ""):
    mesh = load_asset(mesh_path)
    if not mesh:
        return None

    actor = spawn_actor(unreal.StaticMeshActor, location, rotation)
    if not actor:
        return None

    actor.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
    sm_component = actor.get_component_by_class(unreal.StaticMeshComponent)
    if sm_component:
        sm_component.set_editor_property("static_mesh", mesh)
        sm_component.set_editor_property("mobility", unreal.ComponentMobility.STATIC)

    if label:
        actor.set_actor_label(label)
    return actor



def spawn_player_start(location, yaw: float, team_id: int, index: int):
    actor = spawn_actor(unreal.PlayerStart, location, unreal.Rotator(0.0, yaw, 0.0))
    if not actor:
        return None

    actor.set_actor_label(f"PS_T{team_id}_{index:02d}")
    actor.set_editor_property("tags", [unreal.Name(f"Team{team_id}"), unreal.Name("MP")])
    return actor



def spawn_callout(location, text: str):
    callout = spawn_actor(unreal.TextRenderActor, location, unreal.Rotator(0.0, 0.0, 0.0))
    if not callout:
        return None

    callout.set_actor_label(f"Callout_{text.replace(' ', '_')}")
    component = callout.get_component_by_class(unreal.TextRenderComponent)
    if component:
        component.set_editor_property("text", unreal.Text(text))
        component.set_editor_property("horizontal_alignment", unreal.HorizTextAligment.EHTA_CENTER)
        component.set_editor_property("vertical_alignment", unreal.VerticalTextAligment.EVRTA_TEXT_CENTER)
        component.set_editor_property("world_size", 150.0)
        component.set_editor_property("x_scale", 1.0)
        component.set_editor_property("y_scale", 1.0)
        component.set_editor_property("text_render_color", unreal.Color(80, 220, 255, 255))

    return callout



def add_common_lighting(map_name: str, rainy: bool):
    directional = spawn_actor(unreal.DirectionalLight, unreal.Vector(0.0, 0.0, 4000.0), unreal.Rotator(-35.0, 35.0, 0.0))
    if directional:
        directional.set_actor_label("Key_DirectionalLight")

    skylight = spawn_actor(unreal.SkyLight, unreal.Vector(0.0, 0.0, 800.0))
    if skylight:
        skylight.set_actor_label("Key_Skylight")

    fog = spawn_actor(unreal.ExponentialHeightFog, unreal.Vector(0.0, 0.0, 0.0))
    if fog:
        fog.set_actor_label("Atmosphere_HeightFog")
        fog_comp = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
        if fog_comp:
            fog_comp.set_editor_property("fog_density", 0.02 if rainy else 0.012)
            fog_comp.set_editor_property("fog_height_falloff", 0.18)

    if rainy:
        rain_volume = spawn_mesh(
            MESH_CUBE,
            unreal.Vector(0.0, 0.0, 2200.0),
            scale=(140.0, 100.0, 8.0),
            label="Rain_VolumePlaceholder",
        )
        if rain_volume:
            rain_volume.set_actor_hidden_in_game(True)

    if map_name == "Lvl_MP_NeonDistrict":
        if EDITOR_ASSET_LIBRARY.does_asset_exist(NEON_VISUAL_DIRECTOR_BP):
            visual_director_class = get_blueprint_generated_class(NEON_VISUAL_DIRECTOR_BP)
            if visual_director_class:
                actor = spawn_actor(visual_director_class, unreal.Vector(0.0, 0.0, 200.0))
                if actor:
                    actor.set_actor_label("NeonDistrict_VisualDirector")



def build_boundary_walls():
    # Long side walls
    spawn_mesh(MESH_CUBE, unreal.Vector(0.0, 7600.0, 500.0), (130.0, 2.0, 12.0), label="Boundary_North")
    spawn_mesh(MESH_CUBE, unreal.Vector(0.0, -7600.0, 500.0), (130.0, 2.0, 12.0), label="Boundary_South")

    # End walls behind spawns
    spawn_mesh(MESH_CUBE, unreal.Vector(-12200.0, 0.0, 500.0), (2.0, 82.0, 12.0), label="Boundary_West")
    spawn_mesh(MESH_CUBE, unreal.Vector(12200.0, 0.0, 500.0), (2.0, 82.0, 12.0), label="Boundary_East")



def build_three_lane_foundation():
    # Floors
    spawn_mesh(MESH_CHAMFER, unreal.Vector(0.0, 0.0, -120.0), (120.0, 22.0, 1.4), label="Lane_Center_Floor")
    spawn_mesh(MESH_CHAMFER, unreal.Vector(0.0, 4500.0, -120.0), (120.0, 18.0, 1.3), label="Lane_Left_Floor")
    spawn_mesh(MESH_CHAMFER, unreal.Vector(0.0, -4500.0, -120.0), (120.0, 18.0, 1.3), label="Lane_Right_Floor")

    # Lane connectors for rotations
    spawn_mesh(MESH_CHAMFER, unreal.Vector(-2500.0, 0.0, -120.0), (16.0, 52.0, 1.0), label="Connector_West")
    spawn_mesh(MESH_CHAMFER, unreal.Vector(2500.0, 0.0, -120.0), (16.0, 52.0, 1.0), label="Connector_East")

    # Spawn-safe blockers
    for x in (-9300.0, 9300.0):
        spawn_mesh(MESH_CUBE, unreal.Vector(x, 0.0, 350.0), (4.0, 20.0, 7.0), label=f"Spawn_Shield_{'West' if x < 0 else 'East'}")



def build_cover_line(lane_y: float, x_positions, scale=(2.4, 2.4, 2.0), mesh=MESH_CUBE, prefix="Cover"):
    for idx, x in enumerate(x_positions):
        spawn_mesh(mesh, unreal.Vector(x, lane_y, 120.0), scale, label=f"{prefix}_{lane_y}_{idx:02d}")



def spawn_mp_objectives(layout: dict):
    flag_class = get_blueprint_generated_class(FLAG_BP)
    bomb_class = get_blueprint_generated_class(BOMB_BP)
    site_class = get_blueprint_generated_class(SITE_BP)

    if not flag_class or not bomb_class or not site_class:
        warn("Objective blueprints are missing. Map created without objective actors.")
        return

    flag_t0 = spawn_actor(flag_class, unreal.Vector(layout["flag_t0"][0], layout["flag_t0"][1], layout["flag_t0"][2]))
    flag_t1 = spawn_actor(flag_class, unreal.Vector(layout["flag_t1"][0], layout["flag_t1"][1], layout["flag_t1"][2]))

    if flag_t0:
        flag_t0.set_actor_label("CTF_Flag_Team0")
        flag_t0.set_editor_property("flag_team_id", 0)
    if flag_t1:
        flag_t1.set_actor_label("CTF_Flag_Team1")
        flag_t1.set_editor_property("flag_team_id", 1)

    if hasattr(unreal, "COFlagCaptureZoneActor"):
        zone_t0 = spawn_actor(unreal.COFlagCaptureZoneActor, unreal.Vector(layout["capture_t0"][0], layout["capture_t0"][1], layout["capture_t0"][2]))
        zone_t1 = spawn_actor(unreal.COFlagCaptureZoneActor, unreal.Vector(layout["capture_t1"][0], layout["capture_t1"][1], layout["capture_t1"][2]))

        if zone_t0:
            zone_t0.set_actor_label("CTF_CaptureZone_Team0")
            zone_t0.set_editor_property("zone_team_id", 0)
            if flag_t0:
                zone_t0.set_editor_property("friendly_flag", flag_t0)
            if flag_t1:
                zone_t0.set_editor_property("enemy_flag", flag_t1)

        if zone_t1:
            zone_t1.set_actor_label("CTF_CaptureZone_Team1")
            zone_t1.set_editor_property("zone_team_id", 1)
            if flag_t1:
                zone_t1.set_editor_property("friendly_flag", flag_t1)
            if flag_t0:
                zone_t1.set_editor_property("enemy_flag", flag_t0)

    bomb = spawn_actor(bomb_class, unreal.Vector(layout["bomb_spawn"][0], layout["bomb_spawn"][1], layout["bomb_spawn"][2]))
    if bomb:
        bomb.set_actor_label("SnD_Bomb")

    site_a = spawn_actor(site_class, unreal.Vector(layout["site_a"][0], layout["site_a"][1], layout["site_a"][2]))
    site_b = spawn_actor(site_class, unreal.Vector(layout["site_b"][0], layout["site_b"][1], layout["site_b"][2]))

    if site_a:
        site_a.set_actor_label("SnD_Site_A")
        site_a.set_editor_property("site_id", unreal.Name("A"))
        site_a.set_editor_property("defending_team_id", 0)

    if site_b:
        site_b.set_actor_label("SnD_Site_B")
        site_b.set_editor_property("site_id", unreal.Name("B"))
        site_b.set_editor_property("defending_team_id", 0)



def spawn_team_starts(left_spawn_x: float, right_spawn_x: float):
    y_values = [0.0, 1500.0, -1500.0, 3300.0, -3300.0, 5100.0, -5100.0, 6400.0, -6400.0]

    for idx, y in enumerate(y_values, start=1):
        spawn_player_start(unreal.Vector(left_spawn_x, y, 40.0), yaw=0.0, team_id=0, index=idx)
        spawn_player_start(unreal.Vector(right_spawn_x, y, 40.0), yaw=180.0, team_id=1, index=idx)



def place_neon_set_dressing():
    # Boulevard vehicle/barrier rhythm
    build_cover_line(0.0, [-6500.0, -4500.0, -2500.0, 2500.0, 4500.0, 6500.0], scale=(2.0, 3.0, 2.0), prefix="Neon_BoulevardCover")

    # Apartment lane balcony masses
    build_cover_line(4700.0, [-7000.0, -5200.0, -3000.0, 3000.0, 5200.0, 7000.0], scale=(3.0, 1.8, 5.0), mesh=MESH_CHAMFER, prefix="Neon_AptMass")

    # Metro side lane structures
    build_cover_line(-4700.0, [-7200.0, -5400.0, -3600.0, 3600.0, 5400.0, 7200.0], scale=(2.5, 2.5, 3.5), mesh=MESH_CYLINDER, prefix="Neon_MetroPillar")

    # Mid-map billboard scaffold proxy
    spawn_mesh(MESH_CUBE, unreal.Vector(0.0, 0.0, 900.0), (3.0, 20.0, 1.0), label="Neon_BillboardDeck")
    spawn_mesh(MESH_CUBE, unreal.Vector(-500.0, 0.0, 500.0), (0.8, 0.8, 8.0), label="Neon_BillboardSupport_L")
    spawn_mesh(MESH_CUBE, unreal.Vector(500.0, 0.0, 500.0), (0.8, 0.8, 8.0), label="Neon_BillboardSupport_R")

    spawn_callout(unreal.Vector(-7000.0, 0.0, 360.0), "West Boulevard")
    spawn_callout(unreal.Vector(0.0, 0.0, 360.0), "Central Plaza")
    spawn_callout(unreal.Vector(7000.0, 0.0, 360.0), "East Boulevard")
    spawn_callout(unreal.Vector(0.0, 4700.0, 360.0), "Apartments")
    spawn_callout(unreal.Vector(0.0, -4700.0, 360.0), "Metro Lane")



def place_industrial_set_dressing():
    # Container stacks in center dock
    for x in (-6500.0, -3500.0, -500.0, 2500.0, 5500.0):
        spawn_mesh(MESH_CUBE, unreal.Vector(x, -500.0, 150.0), (2.2, 1.6, 2.2), label=f"Ind_Container_{int(x)}_A")
        spawn_mesh(MESH_CUBE, unreal.Vector(x + 900.0, 700.0, 330.0), (2.2, 1.6, 2.2), label=f"Ind_Container_{int(x)}_B")

    # Conveyor/right lane catwalk proxies
    build_cover_line(-4500.0, [-7200.0, -5200.0, -3200.0, 3200.0, 5200.0, 7200.0], scale=(2.8, 1.2, 5.5), mesh=MESH_CHAMFER, prefix="Ind_ConveyorMass")
    spawn_mesh(MESH_RAMP, unreal.Vector(-2200.0, -4500.0, 100.0), (3.0, 6.0, 2.0), rotation=unreal.Rotator(0.0, 0.0, 0.0), label="Ind_CatwalkRamp_W")
    spawn_mesh(MESH_RAMP, unreal.Vector(2200.0, -4500.0, 100.0), (3.0, 6.0, 2.0), rotation=unreal.Rotator(0.0, 180.0, 0.0), label="Ind_CatwalkRamp_E")

    # Tunnel/left lane low visibility pieces
    build_cover_line(4500.0, [-7600.0, -5800.0, -4000.0, 4000.0, 5800.0, 7600.0], scale=(2.0, 2.0, 2.8), mesh=MESH_CYLINDER, prefix="Ind_TunnelColumns")

    # Crane platform proxy
    spawn_mesh(MESH_CHAMFER, unreal.Vector(0.0, 0.0, 1200.0), (4.5, 3.0, 0.7), label="Ind_CranePlatform")
    spawn_mesh(MESH_CUBE, unreal.Vector(0.0, 0.0, 700.0), (0.9, 0.9, 10.0), label="Ind_CraneMast")

    spawn_callout(unreal.Vector(-7000.0, 0.0, 360.0), "Loading Dock")
    spawn_callout(unreal.Vector(0.0, 0.0, 360.0), "Container Yard")
    spawn_callout(unreal.Vector(7000.0, 0.0, 360.0), "Fabrication Bay")
    spawn_callout(unreal.Vector(0.0, 4500.0, 360.0), "Underground Route")
    spawn_callout(unreal.Vector(0.0, -4500.0, 360.0), "Conveyor Lane")



def build_map(map_name: str, set_dressing_fn, objective_layout: dict, rainy: bool = False):
    map_path = f"{MAPS_FOLDER}/{map_name}"

    if EDITOR_ASSET_LIBRARY.does_asset_exist(map_path):
        log(f"Deleting existing map asset to rebuild: {map_path}")
        if not EDITOR_ASSET_LIBRARY.delete_asset(map_path):
            raise RuntimeError(f"Failed to delete existing map {map_path}")

    created = EDITOR_LEVEL_LIBRARY.new_level(map_path)
    if not created:
        raise RuntimeError(f"Failed to create new level: {map_path}")

    build_three_lane_foundation()
    build_boundary_walls()
    spawn_team_starts(-10600.0, 10600.0)
    set_dressing_fn()
    spawn_mp_objectives(objective_layout)
    add_common_lighting(map_name, rainy=rainy)

    if not LEVEL_UTILS.save_current_level():
        raise RuntimeError(f"Failed to save level: {map_path}")

    log(f"Created and saved {map_path}")



def main():
    ensure_directory("/Game/Multiplayer")
    ensure_directory(MAPS_FOLDER)

    neon_layout = {
        "flag_t0": (-9300.0, 1800.0, 80.0),
        "flag_t1": (9300.0, -1800.0, 80.0),
        "capture_t0": (-9000.0, 1800.0, 60.0),
        "capture_t1": (9000.0, -1800.0, 60.0),
        "bomb_spawn": (-8200.0, -300.0, 90.0),
        "site_a": (1200.0, 300.0, 90.0),
        "site_b": (2500.0, -4400.0, 90.0),
    }

    industrial_layout = {
        "flag_t0": (-9300.0, -2200.0, 80.0),
        "flag_t1": (9300.0, 2200.0, 80.0),
        "capture_t0": (-9000.0, -2200.0, 60.0),
        "capture_t1": (9000.0, 2200.0, 60.0),
        "bomb_spawn": (-8000.0, 500.0, 90.0),
        "site_a": (2600.0, -4200.0, 90.0),
        "site_b": (2200.0, 4300.0, 90.0),
    }

    build_map("Lvl_MP_NeonDistrict", place_neon_set_dressing, neon_layout, rainy=True)
    build_map("Lvl_MP_IndustrialComplex", place_industrial_set_dressing, industrial_layout, rainy=False)

    LEVEL_UTILS.save_dirty_packages(True, True)
    log("Generated two multiplayer greybox maps successfully.")


if __name__ == "__main__":
    main()
