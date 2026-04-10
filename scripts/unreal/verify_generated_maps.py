import unreal

MAPS = [
    "/Game/Multiplayer/Maps/Lvl_MP_NeonDistrict",
    "/Game/Multiplayer/Maps/Lvl_MP_IndustrialComplex",
]


def count_by_class(actors, cls):
    return len([a for a in actors if isinstance(a, cls)])


def count_by_name_fragment(actors, fragment: str):
    fragment = fragment.lower()
    return len([a for a in actors if fragment in a.get_class().get_name().lower()])


for map_path in MAPS:
    loaded = unreal.EditorLoadingAndSavingUtils.load_map(map_path)
    if not loaded:
        unreal.log_error(f"[MapVerify] Failed to load {map_path}")
        continue

    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    player_starts = count_by_class(actors, unreal.PlayerStart)
    static_meshes = count_by_class(actors, unreal.StaticMeshActor)
    text_actors = count_by_class(actors, unreal.TextRenderActor)
    bomb_sites = count_by_name_fragment(actors, "bombsite")
    bombs = count_by_name_fragment(actors, "bombobjective")
    flags = count_by_name_fragment(actors, "flagobjective")
    capture_zones = count_by_name_fragment(actors, "flagcapturezone")

    unreal.log(
        f"[MapVerify] {map_path}: actors={len(actors)} player_starts={player_starts} "
        f"static_meshes={static_meshes} text_callouts={text_actors} "
        f"bomb={bombs} bomb_sites={bomb_sites} flags={flags} capture_zones={capture_zones}"
    )