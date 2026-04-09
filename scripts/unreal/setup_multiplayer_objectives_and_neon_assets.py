import unreal


ASSET_TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
EDITOR_ASSET_LIBRARY = unreal.EditorAssetLibrary


def log(message: str) -> None:
    unreal.log(f"[CovertOps1Setup] {message}")


def warn(message: str) -> None:
    unreal.log_warning(f"[CovertOps1Setup] {message}")


def to_text(value: str):
    return unreal.TextLibrary.conv_string_to_text(value)


def ensure_directory(path: str) -> None:
    if not EDITOR_ASSET_LIBRARY.does_directory_exist(path):
        EDITOR_ASSET_LIBRARY.make_directory(path)
        log(f"Created folder: {path}")


def load_asset(asset_path: str):
    if not EDITOR_ASSET_LIBRARY.does_asset_exist(asset_path):
        return None
    return EDITOR_ASSET_LIBRARY.load_asset(asset_path)


def create_or_load_asset(asset_name: str, package_path: str, asset_class, factory):
    asset_path = f"{package_path}/{asset_name}"
    existing = load_asset(asset_path)
    if existing:
        log(f"Using existing asset: {asset_path}")
        return existing, False

    created = ASSET_TOOLS.create_asset(asset_name, package_path, asset_class, factory)
    if not created:
        raise RuntimeError(f"Failed to create asset {asset_path}")

    log(f"Created asset: {asset_path}")
    return created, True


def make_key(key_name: str):
    key = unreal.Key()
    key.set_editor_property("key_name", unreal.Name(key_name))
    return key


def add_key_mapping(imc, action, key_name: str) -> None:
    key = make_key(key_name)

    if hasattr(imc, "unmap_key"):
        try:
            imc.unmap_key(action, key)
        except Exception:
            pass

    if hasattr(imc, "map_key"):
        imc.map_key(action, key)
        log(f"Added mapping in {imc.get_name()}: {key_name}")
    else:
        warn(f"InputMappingContext API map_key missing; could not add {key_name}.")


def create_or_update_interact_action():
    ensure_directory("/Game/Variant_Shooter/Input/Actions")
    ia, _ = create_or_load_asset(
        "IA_Interact",
        "/Game/Variant_Shooter/Input/Actions",
        unreal.InputAction,
        unreal.InputAction_Factory(),
    )

    try:
        ia.set_editor_property("value_type", unreal.InputActionValueType.BOOLEAN)
    except Exception as exc:
        warn(f"Could not set IA_Interact value_type: {exc}")

    imc = load_asset("/Game/Variant_Shooter/Input/IMC_Weapons")
    if not imc:
        warn("IMC_Weapons not found. IA_Interact was created but not mapped.")
    else:
        add_key_mapping(imc, ia, "E")
        add_key_mapping(imc, ia, "Gamepad_FaceButton_Left")
        unreal.EditorAssetLibrary.save_loaded_asset(imc)

    unreal.EditorAssetLibrary.save_loaded_asset(ia)
    return ia


def create_blueprint_child(asset_name: str, package_path: str, parent_class):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    blueprint, _ = create_or_load_asset(asset_name, package_path, unreal.Blueprint, factory)
    unreal.EditorAssetLibrary.save_loaded_asset(blueprint)
    return blueprint


def assign_component_mesh(blueprint, component_property: str, mesh_asset_path: str, scale=None):
    mesh_asset = load_asset(mesh_asset_path)
    if not mesh_asset:
        warn(f"Missing mesh asset: {mesh_asset_path}")
        return

    generated_class = getattr(blueprint, "generated_class", None)
    if callable(generated_class):
        generated_class = generated_class()

    if not generated_class:
        warn(f"Could not resolve generated class for {blueprint.get_name()}")
        return

    cdo = unreal.get_default_object(generated_class)
    try:
        component = cdo.get_editor_property(component_property)
    except Exception as exc:
        warn(f"{blueprint.get_name()} missing component property {component_property}: {exc}")
        return

    if not component:
        warn(f"{blueprint.get_name()} missing component {component_property}")
        return

    component.set_editor_property("static_mesh", mesh_asset)
    if scale:
        component.set_editor_property("relative_scale3d", unreal.Vector(scale[0], scale[1], scale[2]))

    unreal.EditorAssetLibrary.save_loaded_asset(blueprint)
    log(f"Assigned {mesh_asset_path} to {blueprint.get_name()}.{component_property}")


def create_objective_blueprints():
    ensure_directory("/Game/Multiplayer")
    ensure_directory("/Game/Multiplayer/Objectives")

    bp_flag = create_blueprint_child(
        "BP_COFlagObjectiveActor",
        "/Game/Multiplayer/Objectives",
        unreal.COFlagObjectiveActor,
    )
    bp_bomb = create_blueprint_child(
        "BP_COBombObjectiveActor",
        "/Game/Multiplayer/Objectives",
        unreal.COBombObjectiveActor,
    )
    bp_site = create_blueprint_child(
        "BP_COBombSiteActor",
        "/Game/Multiplayer/Objectives",
        unreal.COBombSiteActor,
    )

    assign_component_mesh(
        bp_flag,
        "flag_mesh",
        "/Game/LevelPrototyping/Interactable/Target/Assets/SM_TargetBaseMesh",
        scale=(0.6, 0.6, 1.2),
    )
    assign_component_mesh(
        bp_bomb,
        "bomb_mesh",
        "/Game/Weapons/Rifle/Meshes/SM_Rifle",
        scale=(0.2, 0.2, 0.2),
    )
    assign_component_mesh(
        bp_site,
        "site_mesh",
        "/Game/LevelPrototyping/Meshes/SM_Cylinder",
        scale=(1.2, 1.2, 0.3),
    )


def create_or_update_neon_visual_profile():
    ensure_directory("/Game/Multiplayer/Visual")

    if not hasattr(unreal, "CovertOpsVisualProfileData"):
        warn("CovertOpsVisualProfileData class not available yet. Build C++ and rerun this script.")
        return

    data_asset_factory = unreal.DataAssetFactory()
    data_asset_factory.set_editor_property("data_asset_class", unreal.CovertOpsVisualProfileData)
    profile, _ = create_or_load_asset(
        "DA_Visual_NeonDistrict",
        "/Game/Multiplayer/Visual",
        unreal.CovertOpsVisualProfileData,
        data_asset_factory,
    )

    profile.set_editor_property("map_id", "NeonDistrict")
    profile.set_editor_property("display_name", to_text("Neon District"))
    profile.set_editor_property("atmosphere_description", to_text("Rain-soaked megacity district with high-contrast cyan and magenta combat accents."))
    profile.set_editor_property("target_lux_day", 16000.0)
    profile.set_editor_property("target_lux_night", 4.5)
    profile.set_editor_property("max_bloom", 0.34)
    profile.set_editor_property(
        "accent_palette",
        [
            unreal.LinearColor(0.02, 0.76, 1.0, 1.0),
            unreal.LinearColor(1.0, 0.08, 0.64, 1.0),
            unreal.LinearColor(1.0, 0.65, 0.1, 1.0),
            unreal.LinearColor(0.9, 0.95, 1.0, 1.0),
        ],
    )

    color_grading = profile.get_editor_property("color_grading")
    color_grading.set_editor_property("shadow_tint", unreal.LinearColor(0.93, 0.98, 1.0, 1.0))
    color_grading.set_editor_property("midtone_tint", unreal.LinearColor(1.0, 1.0, 1.0, 1.0))
    color_grading.set_editor_property("highlight_tint", unreal.LinearColor(1.0, 0.97, 0.94, 1.0))
    color_grading.set_editor_property("contrast", 1.18)
    color_grading.set_editor_property("saturation", 1.1)
    profile.set_editor_property("color_grading", color_grading)

    weather = profile.get_editor_property("weather")
    weather.set_editor_property("fog_density", 0.018)
    weather.set_editor_property("fog_height_falloff", 0.12)
    weather.set_editor_property("rain_intensity", 0.78)
    weather.set_editor_property("sandstorm_intensity", 0.0)
    weather.set_editor_property("wetness", 0.82)
    profile.set_editor_property("weather", weather)

    unreal.EditorAssetLibrary.save_loaded_asset(profile)

    if not hasattr(unreal, "CONeonDistrictVisualDirector"):
        warn("CONeonDistrictVisualDirector class not available yet. Build C++ and rerun this script.")
        return

    blueprint = create_blueprint_child("BP_CONeonDistrictVisualDirector", "/Game/Multiplayer/Visual", unreal.CONeonDistrictVisualDirector)
    generated_class = getattr(blueprint, "generated_class", None)
    if callable(generated_class):
        generated_class = generated_class()

    if not generated_class:
        warn("Could not resolve generated class for BP_CONeonDistrictVisualDirector.")
        return

    cdo = unreal.get_default_object(generated_class)
    cdo.set_editor_property("visual_profile", profile)
    cdo.set_editor_property("b_use_night_lighting", True)
    cdo.set_editor_property("b_apply_on_begin_play", True)
    cdo.set_editor_property("b_audit_on_begin_play", True)
    unreal.EditorAssetLibrary.save_loaded_asset(blueprint)

    log("Neon District visual profile + visual director blueprint prepared.")


def main():
    create_or_update_interact_action()
    create_objective_blueprints()
    create_or_update_neon_visual_profile()
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    log("Completed setup for objective interaction and Neon District visual pass.")


if __name__ == "__main__":
    main()
