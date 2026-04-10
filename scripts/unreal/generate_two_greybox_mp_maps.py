import unreal

ASSET_TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
EDITOR_ASSET_LIBRARY = unreal.EditorAssetLibrary
EDITOR_LEVEL_LIBRARY = unreal.EditorLevelLibrary
LEVEL_UTILS = unreal.EditorLoadingAndSavingUtils
MATERIAL_LIB = unreal.MaterialEditingLibrary

MESH_CUBE = "/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube"
MESH_CHAMFER = "/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube"
MESH_CYLINDER = "/Game/LevelPrototyping/Meshes/SM_Cylinder.SM_Cylinder"
MESH_RAMP = "/Game/LevelPrototyping/Meshes/SM_Ramp.SM_Ramp"
MESH_DOOR = "/Game/LevelPrototyping/Interactable/Door/Meshes/SM_Door.SM_Door"
MESH_DOOR_EDGE = "/Game/LevelPrototyping/Interactable/Door/Meshes/SM_DoorFrame_Edge.SM_DoorFrame_Edge"
MESH_GLOW_RING = "/Game/LevelPrototyping/Interactable/JumpPad/Assets/Meshes/SM_CircularBand.SM_CircularBand"
MESH_GLOW_DISC = "/Game/LevelPrototyping/Interactable/JumpPad/Assets/Meshes/SM_CircularGlow.SM_CircularGlow"

MAT_FLAT = "/Game/LevelPrototyping/Materials/M_FlatCol.M_FlatCol"
MAT_GRID = "/Game/LevelPrototyping/Materials/M_PrototypeGrid.M_PrototypeGrid"

MAPS_FOLDER = "/Game/Multiplayer/Maps"
VISUAL_FOLDER = "/Game/Multiplayer/Visual"
VISUAL_MAT_FOLDER = "/Game/Multiplayer/Visual/Materials"

FLAG_BP = "/Game/Multiplayer/Objectives/BP_COFlagObjectiveActor.BP_COFlagObjectiveActor"
BOMB_BP = "/Game/Multiplayer/Objectives/BP_COBombObjectiveActor.BP_COBombObjectiveActor"
SITE_BP = "/Game/Multiplayer/Objectives/BP_COBombSiteActor.BP_COBombSiteActor"
NEON_VISUAL_DIRECTOR_BP = "/Game/Multiplayer/Visual/BP_CONeonDistrictVisualDirector.BP_CONeonDistrictVisualDirector"


def log(message: str) -> None:
    unreal.log(f"[CovertOps1MapGen] {message}")


def warn(message: str) -> None:
    unreal.log_warning(f"[CovertOps1MapGen] {message}")


def safe_set(obj, prop: str, value) -> None:
    try:
        obj.set_editor_property(prop, value)
    except Exception:
        pass


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


def create_or_load_material_instance(
    name: str,
    parent_path: str,
    vector_params: dict,
    scalar_params: dict,
):
    ensure_directory(VISUAL_MAT_FOLDER)
    asset_path = f"{VISUAL_MAT_FOLDER}/{name}"

    instance = unreal.load_asset(asset_path) if EDITOR_ASSET_LIBRARY.does_asset_exist(asset_path) else None
    if not instance:
        parent = load_asset(parent_path)
        if not parent:
            return None

        factory = unreal.MaterialInstanceConstantFactoryNew()
        safe_set(factory, "initial_parent", parent)
        instance = ASSET_TOOLS.create_asset(name, VISUAL_MAT_FOLDER, unreal.MaterialInstanceConstant, factory)
        if not instance:
            warn(f"Failed to create material instance {asset_path}")
            return None

    parent = load_asset(parent_path)
    if parent:
        safe_set(instance, "parent", parent)

    for param_name, color in vector_params.items():
        try:
            MATERIAL_LIB.set_material_instance_vector_parameter_value(
                instance,
                unreal.Name(param_name),
                unreal.LinearColor(color[0], color[1], color[2], color[3]),
            )
        except Exception as exc:
            warn(f"Could not set vector param {param_name} on {name}: {exc}")

    for param_name, value in scalar_params.items():
        try:
            MATERIAL_LIB.set_material_instance_scalar_parameter_value(instance, unreal.Name(param_name), float(value))
        except Exception as exc:
            warn(f"Could not set scalar param {param_name} on {name}: {exc}")

    EDITOR_ASSET_LIBRARY.save_loaded_asset(instance)
    return instance


def build_material_library():
    mats = {}

    mats["common_concrete"] = create_or_load_material_instance(
        "MI_MP_Common_Concrete",
        MAT_FLAT,
        {"Base Color": (0.42, 0.45, 0.49, 1.0)},
        {"Metallic": 0.0, "Roughness": 0.82},
    )
    mats["common_darksteel"] = create_or_load_material_instance(
        "MI_MP_Common_DarkSteel",
        MAT_FLAT,
        {"Base Color": (0.15, 0.16, 0.19, 1.0)},
        {"Metallic": 0.45, "Roughness": 0.35},
    )
    mats["neon_wetasphalt"] = create_or_load_material_instance(
        "MI_MP_Neon_WetAsphalt",
        MAT_FLAT,
        {"Base Color": (0.05, 0.06, 0.08, 1.0)},
        {"Metallic": 0.2, "Roughness": 0.09},
    )
    mats["neon_cyan"] = create_or_load_material_instance(
        "MI_MP_Neon_CyanAccent",
        MAT_FLAT,
        {"Base Color": (0.02, 0.71, 0.95, 1.0)},
        {"Metallic": 0.0, "Roughness": 0.2},
    )
    mats["neon_magenta"] = create_or_load_material_instance(
        "MI_MP_Neon_MagentaAccent",
        MAT_FLAT,
        {"Base Color": (0.92, 0.14, 0.62, 1.0)},
        {"Metallic": 0.0, "Roughness": 0.22},
    )
    mats["neon_grid"] = create_or_load_material_instance(
        "MI_MP_Neon_Linework",
        MAT_GRID,
        {
            "SurfaceColor": (0.08, 0.1, 0.12, 1.0),
            "TopSurfaceColor": (0.12, 0.14, 0.17, 1.0),
            "GridColor": (0.08, 0.72, 0.95, 1.0),
            "TopGridColor": (0.95, 0.2, 0.63, 1.0),
            "SubGridColor": (0.28, 0.34, 0.4, 1.0),
            "TopSubGridGridColor": (0.35, 0.4, 0.48, 1.0),
            "Line Dimensions": (0.09, 0.03, 0.0, 0.0),
        },
        {"Roughness": 0.35, "Grid Size": 65.0, "Sub Grid Number": 3.0, "CircleSize": 0.0},
    )
    mats["industrial_steel"] = create_or_load_material_instance(
        "MI_MP_Industrial_Steel",
        MAT_FLAT,
        {"Base Color": (0.29, 0.31, 0.34, 1.0)},
        {"Metallic": 0.6, "Roughness": 0.28},
    )
    mats["industrial_rust"] = create_or_load_material_instance(
        "MI_MP_Industrial_Rust",
        MAT_FLAT,
        {"Base Color": (0.4, 0.23, 0.11, 1.0)},
        {"Metallic": 0.12, "Roughness": 0.64},
    )
    mats["industrial_hazard"] = create_or_load_material_instance(
        "MI_MP_Industrial_Hazard",
        MAT_FLAT,
        {"Base Color": (0.88, 0.56, 0.08, 1.0)},
        {"Metallic": 0.0, "Roughness": 0.43},
    )

    return mats


def spawn_actor(actor_class, location, rotation=unreal.Rotator(0.0, 0.0, 0.0)):
    return EDITOR_LEVEL_LIBRARY.spawn_actor_from_class(actor_class, location, rotation)


def apply_material(static_mesh_actor, material_asset):
    if not static_mesh_actor or not material_asset:
        return

    component = static_mesh_actor.get_component_by_class(unreal.StaticMeshComponent)
    if component:
        try:
            component.set_material(0, material_asset)
        except Exception:
            pass


def spawn_mesh(
    mesh_path: str,
    location,
    scale,
    rotation=unreal.Rotator(0.0, 0.0, 0.0),
    label: str = "",
    material_asset=None,
):
    mesh = load_asset(mesh_path)
    if not mesh:
        return None

    actor = spawn_actor(unreal.StaticMeshActor, location, rotation)
    if not actor:
        return None

    actor.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
    sm_component = actor.get_component_by_class(unreal.StaticMeshComponent)
    if sm_component:
        safe_set(sm_component, "static_mesh", mesh)
        safe_set(sm_component, "mobility", unreal.ComponentMobility.STATIC)

    if material_asset:
        apply_material(actor, material_asset)

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
        safe_set(component, "text", unreal.Text(text))
        safe_set(component, "horizontal_alignment", unreal.HorizTextAligment.EHTA_CENTER)
        safe_set(component, "vertical_alignment", unreal.VerticalTextAligment.EVRTA_TEXT_CENTER)
        safe_set(component, "world_size", 150.0)
        safe_set(component, "text_render_color", unreal.Color(80, 220, 255, 255))

    return callout


def configure_directional_light(light_actor, intensity: float, color: unreal.Color, pitch: float, yaw: float):
    if not light_actor:
        return

    light_actor.set_actor_rotation(unreal.Rotator(pitch, yaw, 0.0), False)
    comp = light_actor.get_component_by_class(unreal.DirectionalLightComponent)
    if comp:
        safe_set(comp, "intensity", intensity)
        safe_set(comp, "light_color", color)
        safe_set(comp, "cast_shadows", True)


def configure_sky_light(actor, intensity: float):
    if not actor:
        return
    comp = actor.get_component_by_class(unreal.SkyLightComponent)
    if comp:
        safe_set(comp, "intensity", intensity)
        safe_set(comp, "real_time_capture", True)


def configure_height_fog(actor, density: float, falloff: float, inscatter=None):
    if not actor:
        return
    comp = actor.get_component_by_class(unreal.ExponentialHeightFogComponent)
    if comp:
        safe_set(comp, "fog_density", density)
        safe_set(comp, "fog_height_falloff", falloff)
        safe_set(comp, "volumetric_fog", True)
        if inscatter:
            safe_set(comp, "fog_inscattering_color", inscatter)


def add_post_process_profile(name: str, saturation: float, contrast: float, bloom: float, vignette: float, exposure_min: float, exposure_max: float):
    ppv = spawn_actor(unreal.PostProcessVolume, unreal.Vector(0.0, 0.0, 0.0))
    if not ppv:
        return

    ppv.set_actor_label(name)
    safe_set(ppv, "unbound", True)

    settings = ppv.get_editor_property("settings")
    safe_set(settings, "b_override_color_saturation", True)
    safe_set(settings, "b_override_color_contrast", True)
    safe_set(settings, "b_override_bloom_intensity", True)
    safe_set(settings, "b_override_vignette_intensity", True)
    safe_set(settings, "b_override_auto_exposure_min_brightness", True)
    safe_set(settings, "b_override_auto_exposure_max_brightness", True)

    sat = unreal.Vector4(saturation, saturation, saturation, 1.0)
    con = unreal.Vector4(contrast, contrast, contrast, 1.0)
    safe_set(settings, "color_saturation", sat)
    safe_set(settings, "color_contrast", con)
    safe_set(settings, "bloom_intensity", bloom)
    safe_set(settings, "vignette_intensity", vignette)
    safe_set(settings, "auto_exposure_min_brightness", exposure_min)
    safe_set(settings, "auto_exposure_max_brightness", exposure_max)

    safe_set(ppv, "settings", settings)


def spawn_point_light(location, intensity, radius, color, label):
    point = spawn_actor(unreal.PointLight, location)
    if not point:
        return None
    point.set_actor_label(label)
    comp = point.get_component_by_class(unreal.PointLightComponent)
    if comp:
        safe_set(comp, "intensity", intensity)
        safe_set(comp, "attenuation_radius", radius)
        safe_set(comp, "light_color", color)
        safe_set(comp, "cast_shadows", True)
    return point


def spawn_spot_light(location, rotation, intensity, radius, color, inner_cone, outer_cone, label):
    spot = spawn_actor(unreal.SpotLight, location, rotation)
    if not spot:
        return None
    spot.set_actor_label(label)
    comp = spot.get_component_by_class(unreal.SpotLightComponent)
    if comp:
        safe_set(comp, "intensity", intensity)
        safe_set(comp, "attenuation_radius", radius)
        safe_set(comp, "light_color", color)
        safe_set(comp, "inner_cone_angle", inner_cone)
        safe_set(comp, "outer_cone_angle", outer_cone)
        safe_set(comp, "cast_shadows", True)
    return spot


def add_common_atmosphere(night: bool):
    sky_atmosphere = spawn_actor(unreal.SkyAtmosphere, unreal.Vector(0.0, 0.0, 0.0))
    if sky_atmosphere:
        sky_atmosphere.set_actor_label("SkyAtmosphere_Main")

    clouds = spawn_actor(unreal.VolumetricCloud, unreal.Vector(0.0, 0.0, 0.0))
    if clouds:
        clouds.set_actor_label("VolumetricCloud_Main")

    directional = spawn_actor(unreal.DirectionalLight, unreal.Vector(0.0, 0.0, 2000.0))
    if directional:
        directional.set_actor_label("Key_DirectionalLight")
        if night:
            configure_directional_light(directional, 4.2, unreal.Color(152, 184, 255, 255), -32.0, 38.0)
        else:
            configure_directional_light(directional, 11000.0, unreal.Color(242, 245, 255, 255), -48.0, 24.0)

    skylight = spawn_actor(unreal.SkyLight, unreal.Vector(0.0, 0.0, 600.0))
    if skylight:
        skylight.set_actor_label("Key_Skylight")
        configure_sky_light(skylight, 0.75 if night else 1.1)

    fog = spawn_actor(unreal.ExponentialHeightFog, unreal.Vector(0.0, 0.0, 0.0))
    if fog:
        fog.set_actor_label("Atmosphere_HeightFog")
        if night:
            configure_height_fog(fog, 0.022, 0.11, unreal.LinearColor(0.07, 0.12, 0.2, 1.0))
        else:
            configure_height_fog(fog, 0.014, 0.2, unreal.LinearColor(0.42, 0.46, 0.51, 1.0))

    refl = spawn_actor(unreal.SphereReflectionCapture, unreal.Vector(0.0, 0.0, 220.0))
    if refl:
        refl.set_actor_label("ReflectionCapture_Main")


def add_neon_lighting():
    add_post_process_profile(
        name="PPV_NeonDistrict",
        saturation=1.14,
        contrast=1.2,
        bloom=0.34,
        vignette=0.24,
        exposure_min=0.75,
        exposure_max=1.2,
    )

    x_row = [-8800.0, -6800.0, -4800.0, -2800.0, -800.0, 1200.0, 3200.0, 5200.0, 7200.0, 9000.0]
    for idx, x in enumerate(x_row):
        color = unreal.Color(35, 210, 255, 255) if idx % 2 == 0 else unreal.Color(255, 65, 180, 255)
        spawn_point_light(
            unreal.Vector(x, 0.0, 520.0),
            intensity=68000.0,
            radius=1800.0,
            color=color,
            label=f"Neon_BoulevardLight_{idx:02d}",
        )

    for idx, x in enumerate([-5000.0, 0.0, 5000.0]):
        spawn_spot_light(
            location=unreal.Vector(x, 0.0, 1700.0),
            rotation=unreal.Rotator(-70.0, 0.0, 0.0),
            intensity=92000.0,
            radius=3600.0,
            color=unreal.Color(160, 200, 255, 255),
            inner_cone=20.0,
            outer_cone=46.0,
            label=f"Neon_BillboardSpot_{idx:02d}",
        )


def add_industrial_lighting():
    add_post_process_profile(
        name="PPV_IndustrialComplex",
        saturation=0.98,
        contrast=1.12,
        bloom=0.17,
        vignette=0.2,
        exposure_min=0.9,
        exposure_max=1.45,
    )

    for idx, x in enumerate([-8600.0, -6200.0, -3800.0, -1400.0, 1000.0, 3400.0, 5800.0, 8200.0]):
        spawn_point_light(
            unreal.Vector(x, -4100.0, 680.0),
            intensity=54000.0,
            radius=1700.0,
            color=unreal.Color(255, 195, 90, 255),
            label=f"Ind_CatwalkLight_{idx:02d}",
        )

    for idx, y in enumerate([-1300.0, 1300.0]):
        spawn_point_light(
            unreal.Vector(0.0, y, 880.0),
            intensity=42000.0,
            radius=2200.0,
            color=unreal.Color(255, 92, 72, 255),
            label=f"Ind_WarningLight_{idx:02d}",
        )


def build_boundary_walls(material):
    spawn_mesh(MESH_CUBE, unreal.Vector(0.0, 7600.0, 500.0), (130.0, 2.0, 12.0), label="Boundary_North", material_asset=material)
    spawn_mesh(MESH_CUBE, unreal.Vector(0.0, -7600.0, 500.0), (130.0, 2.0, 12.0), label="Boundary_South", material_asset=material)
    spawn_mesh(MESH_CUBE, unreal.Vector(-12200.0, 0.0, 500.0), (2.0, 82.0, 12.0), label="Boundary_West", material_asset=material)
    spawn_mesh(MESH_CUBE, unreal.Vector(12200.0, 0.0, 500.0), (2.0, 82.0, 12.0), label="Boundary_East", material_asset=material)


def build_three_lane_foundation(floor_mat, connector_mat):
    spawn_mesh(MESH_CHAMFER, unreal.Vector(0.0, 0.0, -120.0), (120.0, 22.0, 1.4), label="Lane_Center_Floor", material_asset=floor_mat)
    spawn_mesh(MESH_CHAMFER, unreal.Vector(0.0, 4500.0, -120.0), (120.0, 18.0, 1.3), label="Lane_Left_Floor", material_asset=floor_mat)
    spawn_mesh(MESH_CHAMFER, unreal.Vector(0.0, -4500.0, -120.0), (120.0, 18.0, 1.3), label="Lane_Right_Floor", material_asset=floor_mat)

    spawn_mesh(MESH_CHAMFER, unreal.Vector(-2500.0, 0.0, -120.0), (16.0, 52.0, 1.0), label="Connector_West", material_asset=connector_mat)
    spawn_mesh(MESH_CHAMFER, unreal.Vector(2500.0, 0.0, -120.0), (16.0, 52.0, 1.0), label="Connector_East", material_asset=connector_mat)

    for x in (-9300.0, 9300.0):
        spawn_mesh(
            MESH_CUBE,
            unreal.Vector(x, 0.0, 350.0),
            (4.0, 20.0, 7.0),
            label=f"Spawn_Shield_{'West' if x < 0 else 'East'}",
            material_asset=connector_mat,
        )


def build_cover_line(lane_y: float, x_positions, material_asset, scale=(2.4, 2.4, 2.0), mesh=MESH_CUBE, prefix="Cover"):
    for idx, x in enumerate(x_positions):
        spawn_mesh(mesh, unreal.Vector(x, lane_y, 120.0), scale, label=f"{prefix}_{lane_y}_{idx:02d}", material_asset=material_asset)


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


def place_neon_set_dressing(mats):
    build_cover_line(0.0, [-6500.0, -4500.0, -2500.0, 2500.0, 4500.0, 6500.0], material_asset=mats["common_darksteel"], scale=(2.0, 3.0, 2.0), prefix="Neon_BoulevardCover")
    build_cover_line(4700.0, [-7000.0, -5200.0, -3000.0, 3000.0, 5200.0, 7000.0], material_asset=mats["neon_grid"], scale=(3.0, 1.8, 5.0), mesh=MESH_CHAMFER, prefix="Neon_AptMass")
    build_cover_line(-4700.0, [-7200.0, -5400.0, -3600.0, 3600.0, 5400.0, 7200.0], material_asset=mats["common_concrete"], scale=(2.5, 2.5, 3.5), mesh=MESH_CYLINDER, prefix="Neon_MetroPillar")

    spawn_mesh(MESH_CUBE, unreal.Vector(0.0, 0.0, 900.0), (3.0, 20.0, 1.0), label="Neon_BillboardDeck", material_asset=mats["common_darksteel"])
    spawn_mesh(MESH_CUBE, unreal.Vector(-500.0, 0.0, 500.0), (0.8, 0.8, 8.0), label="Neon_BillboardSupport_L", material_asset=mats["common_darksteel"])
    spawn_mesh(MESH_CUBE, unreal.Vector(500.0, 0.0, 500.0), (0.8, 0.8, 8.0), label="Neon_BillboardSupport_R", material_asset=mats["common_darksteel"])

    for i, x in enumerate([-7600.0, -4400.0, -1200.0, 1800.0, 5000.0, 7800.0]):
        accent = mats["neon_cyan"] if i % 2 == 0 else mats["neon_magenta"]
        spawn_mesh(MESH_GLOW_RING, unreal.Vector(x, 1600.0, 80.0), (3.2, 3.2, 1.0), label=f"Neon_SignRing_{i:02d}", material_asset=accent)
        spawn_mesh(MESH_DOOR, unreal.Vector(x + 400.0, -1600.0, 140.0), (2.0, 2.0, 2.0), label=f"Neon_Door_{i:02d}", material_asset=mats["common_darksteel"])

    spawn_mesh(MESH_GLOW_DISC, unreal.Vector(-1200.0, -4200.0, 60.0), (5.0, 5.0, 1.0), label="Neon_MetroGlow_A", material_asset=mats["neon_cyan"])
    spawn_mesh(MESH_GLOW_DISC, unreal.Vector(2800.0, -4200.0, 60.0), (5.0, 5.0, 1.0), label="Neon_MetroGlow_B", material_asset=mats["neon_magenta"])

    spawn_callout(unreal.Vector(-7000.0, 0.0, 360.0), "West Boulevard")
    spawn_callout(unreal.Vector(0.0, 0.0, 360.0), "Central Plaza")
    spawn_callout(unreal.Vector(7000.0, 0.0, 360.0), "East Boulevard")
    spawn_callout(unreal.Vector(0.0, 4700.0, 360.0), "Apartments")
    spawn_callout(unreal.Vector(0.0, -4700.0, 360.0), "Metro Lane")


def place_industrial_set_dressing(mats):
    for x in (-6500.0, -3500.0, -500.0, 2500.0, 5500.0):
        spawn_mesh(MESH_CUBE, unreal.Vector(x, -500.0, 150.0), (2.2, 1.6, 2.2), label=f"Ind_Container_{int(x)}_A", material_asset=mats["industrial_steel"])
        spawn_mesh(MESH_CUBE, unreal.Vector(x + 900.0, 700.0, 330.0), (2.2, 1.6, 2.2), label=f"Ind_Container_{int(x)}_B", material_asset=mats["industrial_rust"])

    build_cover_line(-4500.0, [-7200.0, -5200.0, -3200.0, 3200.0, 5200.0, 7200.0], material_asset=mats["industrial_steel"], scale=(2.8, 1.2, 5.5), mesh=MESH_CHAMFER, prefix="Ind_ConveyorMass")
    spawn_mesh(MESH_RAMP, unreal.Vector(-2200.0, -4500.0, 100.0), (3.0, 6.0, 2.0), rotation=unreal.Rotator(0.0, 0.0, 0.0), label="Ind_CatwalkRamp_W", material_asset=mats["industrial_hazard"])
    spawn_mesh(MESH_RAMP, unreal.Vector(2200.0, -4500.0, 100.0), (3.0, 6.0, 2.0), rotation=unreal.Rotator(0.0, 180.0, 0.0), label="Ind_CatwalkRamp_E", material_asset=mats["industrial_hazard"])

    build_cover_line(4500.0, [-7600.0, -5800.0, -4000.0, 4000.0, 5800.0, 7600.0], material_asset=mats["industrial_rust"], scale=(2.0, 2.0, 2.8), mesh=MESH_CYLINDER, prefix="Ind_TunnelColumns")

    spawn_mesh(MESH_CHAMFER, unreal.Vector(0.0, 0.0, 1200.0), (4.5, 3.0, 0.7), label="Ind_CranePlatform", material_asset=mats["industrial_steel"])
    spawn_mesh(MESH_CUBE, unreal.Vector(0.0, 0.0, 700.0), (0.9, 0.9, 10.0), label="Ind_CraneMast", material_asset=mats["industrial_steel"])

    for i, x in enumerate([-7600.0, -5000.0, -2400.0, 1000.0, 3600.0, 6200.0, 8600.0]):
        spawn_mesh(MESH_DOOR_EDGE, unreal.Vector(x, 1200.0, 160.0), (2.0, 2.0, 2.3), label=f"Ind_Beam_{i:02d}", material_asset=mats["industrial_hazard"])

    spawn_callout(unreal.Vector(-7000.0, 0.0, 360.0), "Loading Dock")
    spawn_callout(unreal.Vector(0.0, 0.0, 360.0), "Container Yard")
    spawn_callout(unreal.Vector(7000.0, 0.0, 360.0), "Fabrication Bay")
    spawn_callout(unreal.Vector(0.0, 4500.0, 360.0), "Underground Route")
    spawn_callout(unreal.Vector(0.0, -4500.0, 360.0), "Conveyor Lane")


def maybe_spawn_neon_visual_director():
    if EDITOR_ASSET_LIBRARY.does_asset_exist(NEON_VISUAL_DIRECTOR_BP):
        visual_director_class = get_blueprint_generated_class(NEON_VISUAL_DIRECTOR_BP)
        if visual_director_class:
            actor = spawn_actor(visual_director_class, unreal.Vector(0.0, 0.0, 200.0))
            if actor:
                actor.set_actor_label("NeonDistrict_VisualDirector")


def build_map(map_name: str, set_dressing_fn, objective_layout: dict, mats, night: bool):
    map_path = f"{MAPS_FOLDER}/{map_name}"

    if EDITOR_ASSET_LIBRARY.does_asset_exist(map_path):
        log(f"Deleting existing map asset to rebuild: {map_path}")
        if not EDITOR_ASSET_LIBRARY.delete_asset(map_path):
            raise RuntimeError(f"Failed to delete existing map {map_path}")

    created = EDITOR_LEVEL_LIBRARY.new_level(map_path)
    if not created:
        raise RuntimeError(f"Failed to create new level: {map_path}")

    if night:
        floor_mat = mats["neon_wetasphalt"]
        connector_mat = mats["neon_grid"]
        boundary_mat = mats["common_darksteel"]
    else:
        floor_mat = mats["common_concrete"]
        connector_mat = mats["industrial_steel"]
        boundary_mat = mats["industrial_rust"]

    build_three_lane_foundation(floor_mat=floor_mat, connector_mat=connector_mat)
    build_boundary_walls(material=boundary_mat)
    spawn_team_starts(-10600.0, 10600.0)
    set_dressing_fn(mats)
    spawn_mp_objectives(objective_layout)

    add_common_atmosphere(night=night)
    if night:
        add_neon_lighting()
        maybe_spawn_neon_visual_director()
    else:
        add_industrial_lighting()

    if not LEVEL_UTILS.save_current_level():
        raise RuntimeError(f"Failed to save level: {map_path}")

    log(f"Created and saved {map_path}")


def main():
    ensure_directory("/Game/Multiplayer")
    ensure_directory(MAPS_FOLDER)
    ensure_directory(VISUAL_FOLDER)
    ensure_directory(VISUAL_MAT_FOLDER)

    mats = build_material_library()

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

    build_map("Lvl_MP_NeonDistrict", place_neon_set_dressing, neon_layout, mats=mats, night=True)
    build_map("Lvl_MP_IndustrialComplex", place_industrial_set_dressing, industrial_layout, mats=mats, night=False)

    LEVEL_UTILS.save_dirty_packages(True, True)
    log("Generated two multiplayer maps with polished visual pass.")


if __name__ == "__main__":
    main()
