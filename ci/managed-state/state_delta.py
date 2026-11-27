"""Carry an exact main-line ancestor across fully tracked input changes."""

from hashlib import sha256
from subprocess import CalledProcessError

from state_snapshot import OID, authoritative_units, git, load, safe_tu
from state_evidence import LIMIT, cached_pairs, header

# These are orchestration and publication paths. The comparison inputs and
# implementation retain full-refresh semantics. state.yml can be carried only
# across the reviewed producer-v2 to resume transport revision.
REPORT_DIRS = ("docs/", "worklist/", "ci/state/", "ci/progress/", "ci/alarms/",
               "ci/nightly/", "ci/landing/", "ci/admission/", "ci/header/", "ci/public-text/", "ci/maint/",
               "ci/managed-check/tests/", "ci/queue/", "ci/control-plane/", ".github/workflows/")
STATE_FLOW = ".github/workflows/state.yml"
TRANSPORT = {"ci/managed-state/state_delta.py", "ci/managed-state/state_fetch.py",
             "ci/managed-state/state_workflow.py",
             "ci/managed-state/recovery-artifacts.json", "ci/managed-state/state_recover.py",
             "ci/managed-state/state_transport.py", "ci/managed-state/state_origin.py",
             "ci/managed-state/bundle.sha256",
             "ci/README.md"}
RESUME_MIGRATION = ("5df8abc2a97aef390b5a0256a15e163ae4a312566c0ea893187da4a254aea8fc", "14286c7910816519777d5a46c720605d142dff009f5751843d4e4c5263e2111f")
HYDRATION_MIGRATION = ("c5495ba4c03861220f77ddc48cd6533617fde93b869d51a0accf479fcc760bcb", "ec1ec3a1489c459a4513cfef8937101998a740667c0662d15e4df00f102bce61")
OVERCAP_EXIT_MIGRATION = ("ec1ec3a1489c459a4513cfef8937101998a740667c0662d15e4df00f102bce61", "ad0a65303832e04fc813d9a49d875b645f47e6d70b3422ba50d141914487e62d")
# The seed run's default cap (717cbe22) is measured by that seed; this step only adds the automatic seed start.
AUTO_SEED_MIGRATION = ("cf1c93deac7c4d2665d753f9847ad0a6380fca1e3dab49188f9168403902e10a", "348fd37762dd358d3af7f9070d42843be2f2bc6b0d38f42537b7b3e00e3eee95")
# Main is measured on the pull request compiler runner (vars.BFME_PR_RUNNER), so both use one image pin.
STATE_RUNNER_MIGRATION = ("348fd37762dd358d3af7f9070d42843be2f2bc6b0d38f42537b7b3e00e3eee95", "63b75ea948a6d5ff29a3dedeab12551f4ef7793f50b43ede48480f5f04b4b256")
# Configurable max-parallel and in-run seed catch-up refresh.
SEED_LIFECYCLE_MIGRATION = ("63b75ea948a6d5ff29a3dedeab12551f4ef7793f50b43ede48480f5f04b4b256", "2edede02a18485377a6092c309231a880013906d22238f49a6ad276a70ab8bc7")
SEED_YAML_FIX_MIGRATION = ("2edede02a18485377a6092c309231a880013906d22238f49a6ad276a70ab8bc7", "f271e82623b1fca7d91d073b3015638c2026e5d9ad8302460bdc26d82f753a33")
STATE_FLOW_MIGRATIONS = {RESUME_MIGRATION, HYDRATION_MIGRATION, OVERCAP_EXIT_MIGRATION, AUTO_SEED_MIGRATION,
                         STATE_RUNNER_MIGRATION, SEED_LIFECYCLE_MIGRATION, SEED_YAML_FIX_MIGRATION}
# The one-shot PR orchestration change leaves every comparison input intact.
COMPACT_GRAPH = {
    "ci/managed-check/landing_read.py": (
        "fcbc657e031cff9f496470c5ad3fa7d01080357d3f277381584c48d35a3ce89f",
        "c156b8a77e440fa8f87d017389d8aea9e164e236704eba2baf26d190821b1cc1",
    ),
    "ci/managed-check/github_io.py": (
        "3a5c9373f5bb105a71b4f00a730a5361ac7b238372ccf7e7baadb51e17c6b744",
        "af57df171f74d66f557535dfe2b702c194bd51219828060b60dca6ff210c5f77",
    ),
    "ci/managed-check/bundle.sha256": (
        "8e8f38f80f0327372967ae319b28261bca972d0435dd21465875b03806fd9f12",
        "21c6f500a947b2b957366fa346de2864cb3d017901e0e528bfd50220589abf82",
    ),
}
AUTOMATIC_GRAPH = {
    "ci/managed-check/wide_recovery.py": (
        '43ab19a7fd87bbf9e00c7fecdaba498b1c0da45e749a692ae383db7e4247af7d',
        'af69c85ba0829c103afbcfe68d8b1788ee23ea34af0d87bb3f503f6734e4e433',
    ),
    "ci/managed-check/bundle.sha256": (
        '21c6f500a947b2b957366fa346de2864cb3d017901e0e528bfd50220589abf82',
        'ef8d015ae4a4c3b169b9572299f4171b68e4661b22ce63bf2bc121accb2fb336',
    ),
}
# A retained checkpoint can precede either migration, or both of them.
BUNDLE = "ci/managed-check/bundle.sha256"
COMBINED_GRAPH = COMPACT_GRAPH | AUTOMATIC_GRAPH | {
    path: (COMPACT_GRAPH[path][0], AUTOMATIC_GRAPH[path][1])
    for path in COMPACT_GRAPH.keys() & AUTOMATIC_GRAPH.keys()
}
TRANSPORT_LOG_GRAPH = {
    "ci/managed-check/github_io.py": (
        "af57df171f74d66f557535dfe2b702c194bd51219828060b60dca6ff210c5f77",
        "c6c89dbd8ec2923e8d954d09d79b23fd344e1d2be9b0c90f332f9abe16132228",
    ),
    "ci/managed-check/bundle.sha256": (
        "ef8d015ae4a4c3b169b9572299f4171b68e4661b22ce63bf2bc121accb2fb336",
        "9f3ebd824a6efbef574e88ec7a738df50c9f0985056af1055ef6804fe6014112",
    ),
}


def advance_graph(earlier, later):
    overlap = earlier.keys() & later.keys()
    if any(earlier[path][1] != later[path][0] for path in overlap):
        raise ValueError("Orchestration graph preimages do not form a continuous chain.")
    return earlier | later | {path: (earlier[path][0], later[path][1]) for path in overlap}


RESERVE_AND_LOG_GRAPH = advance_graph(AUTOMATIC_GRAPH, TRANSPORT_LOG_GRAPH)
COMBINED_LOG_GRAPH = advance_graph(COMBINED_GRAPH, TRANSPORT_LOG_GRAPH)
GRAPH_CHANGES = (COMPACT_GRAPH, AUTOMATIC_GRAPH, COMBINED_GRAPH,
                 TRANSPORT_LOG_GRAPH, RESERVE_AND_LOG_GRAPH, COMBINED_LOG_GRAPH)
INLINE_BINDING_GRAPH = {
    "ci/managed-check/inline_virtual/inline_virtual_receipt.py": (
        "7be4b2a0b897049c5cdbac32484d50ed794d34a112ff8f08c44e292483ca82ce",
        "5b4f047643182c119cc83ba467453a5c007dbe8173a0d792b95dcc6aa2163fc1",
    ),
    "ci/managed-check/virtual_atoms.py": (
        "e30a1ca021e2685efafbc0a0643b62e51ba85efd905526d06ab14c89ee19b9d0",
        "f5924475fecdda9d79eec784041ebd6e509d7f75b21a7ed087b8c9d01bd3aeaa",
    ),
    "ci/managed-check/bundle.sha256": (
        "9f3ebd824a6efbef574e88ec7a738df50c9f0985056af1055ef6804fe6014112",
        "d29b124bdb33956c3af86aed8f8ca50f36a1877acf4a00d291ba44eb852ad6a4",
    ),
}

GRAPH_CHANGES += (INLINE_BINDING_GRAPH,) + tuple(
    advance_graph(previous, INLINE_BINDING_GRAPH)
    for previous in (TRANSPORT_LOG_GRAPH, RESERVE_AND_LOG_GRAPH, COMBINED_LOG_GRAPH)
)
GRAPH_PATHS = set(COMBINED_LOG_GRAPH) | set(INLINE_BINDING_GRAPH)
ADMISSION_GRAPH = {
    "ci/managed-check/github_io.py": (
        'c6c89dbd8ec2923e8d954d09d79b23fd344e1d2be9b0c90f332f9abe16132228',
        '4a413fed5760e8e14e12c779961939d24111991ad7c888a3244a21810850edfa',
    ),
    "ci/managed-check/intake.py": (
        '3391fb495dccd4cab7bf9610d337c25696e9f56a27985faad5fab26def666b18',
        '4b8fa6c3272a24ed002fc6c16af5dccc0e4416fe17259a73a08f845a48028d04',
    ),
    "ci/managed-check/fork_admission.py": (
        None,
        '9d5d811357515f81e46ee729a1ced768846042f34711d1817712fcded00f45d7',
    ),
    "ci/managed-check/bundle.sha256": (
        'd29b124bdb33956c3af86aed8f8ca50f36a1877acf4a00d291ba44eb852ad6a4',
        'ca92c4cf6cd894d669c2c9876fee5f383f94d93daae987fe9ab7b45b16f7e51b',
    ),
}

GRAPH_CHANGES += (ADMISSION_GRAPH,) + tuple(
    advance_graph(previous, ADMISSION_GRAPH)
    for previous in GRAPH_CHANGES
    if previous[BUNDLE][1] == ADMISSION_GRAPH[BUNDLE][0]
)
GRAPH_PATHS |= set(ADMISSION_GRAPH)

WIDE_CLOCK_GRAPH = {
    "ci/managed-check/compact_matrix.py": (
        "9e5e479a33cfe39bd2cb48d565c00dde51c5ad1adcdae03a50c7f0285683e266",
        "5795922e9a9a034cb2d02e7e686ad3c7efb4a5ac018c2944a2807a47d913afe2",
    ),
    "ci/managed-check/compare.py": (
        "7a3d3976b32c89fa4f656c5a25d882a8172cc232ade04743f99fb84fa6b7e82b",
        "7096d164997ca3ab224a0edc77003898190af33431cdc0640de1e26b9a5072cf",
    ),
    "ci/managed-check/managed_matrix.py": (
        "313a21b37f4ac441aef9cb091893db268a07f709468b0b8f375710f311b3451c",
        "8d80ff76da20ee6d29a9e38502f713532f18c3f42622caed58e885f35201cba1",
    ),
    "ci/managed-check/recovery_artifacts.py": (
        "f81d84ed474e743cc0fdfeec982b919649b2d3d8b6f89c5275fbc896a71d71c9",
        "9f7c44bbf667ff5e64b183bf0d8ca44e5c0077b3131105751257faf10224c924",
    ),
    "ci/managed-check/recovery_broker.py": (
        "4fef8dd2172ea8d1c8f0415bedb9fdd31d2a70356a07f6e1e3ed9cee53ec7639",
        "dd6071bc1fe9e872f239e50e92b97358b349bbe8170e4d896729c805e42be55f",
    ),
    "ci/managed-check/wide.py": (
        "6767f2bb9500c5d3c96251e66bc6c88a8910ac78f71cc3d499e5d2243847ff04",
        "5bd0c4a37ca8fc131f8737fc3ce67047466cb3ebb3e7eb35c9705bf1173370f4",
    ),
    "ci/managed-check/wide_recovery.py": (
        "af69c85ba0829c103afbcfe68d8b1788ee23ea34af0d87bb3f503f6734e4e433",
        "57b74a0ebfa703aaae7c7fa47fcd22610058059cb5e27524b450a7e6c9d981ed",
    ),
    "ci/managed-check/bundle.sha256": (
        "efb05a79f87b5cd21373cbfcf854ecae90cc53fe5d3c381b1408fbc3ef4fa24e",
        "7e52d1b99613f8940f426bcf06fd16e3f4c60aa31c4e3757a9a82aeea99f2cf4",
    ),
}

GRAPH_CHANGES += (WIDE_CLOCK_GRAPH,) + tuple(
    advance_graph(previous, WIDE_CLOCK_GRAPH)
    for previous in GRAPH_CHANGES
    if previous[BUNDLE][1] == WIDE_CLOCK_GRAPH[BUNDLE][0]
)
GRAPH_PATHS |= set(WIDE_CLOCK_GRAPH)

WIDE_ADMISSION_GRAPH = {
    "ci/managed-check/fork_admission.py": (
        "9d5d811357515f81e46ee729a1ced768846042f34711d1817712fcded00f45d7",
        "25badb095dabc6c8a717145f3ea1e14342fbb5982fb10982608e9df7b8e01e34",
    ),
    "ci/managed-check/github_io.py": (
        "9f761ce5ada2907486f1665b98b68650ecc6b764c9685d2d6f168f0aebed3b66",
        "c34a1c9bde3d83f39346bdb8246ee74290cebd5058672d9e24b13388bb02c9e8",
    ),
    "ci/managed-check/intake.py": (
        "509d2ef40980f7c83a5be4f9c096046eb881dda19f4ea2273677e2a07297a493",
        "5c0dc9af03ab8dc962b62aa2ecb5a1b64218c989b387e8bd178aebc352e78bce",
    ),
    "ci/managed-check/recovery_broker.py": (
        "dd6071bc1fe9e872f239e50e92b97358b349bbe8170e4d896729c805e42be55f",
        "22a4cce5519019c86534849930a0f5704a5b7ee9f0b00d4eb06ede8f8869b51d",
    ),
    "ci/managed-check/bundle.sha256": (
        "7e52d1b99613f8940f426bcf06fd16e3f4c60aa31c4e3757a9a82aeea99f2cf4",
        "d8ea452fadaace93fd7785d045aac330da534c80e9402ef2b26d20f7c9d55a24",
    ),
}

GRAPH_CHANGES += (WIDE_ADMISSION_GRAPH,) + tuple(
    advance_graph(previous, WIDE_ADMISSION_GRAPH)
    for previous in GRAPH_CHANGES
    if previous[BUNDLE][1] == WIDE_ADMISSION_GRAPH[BUNDLE][0]
)
GRAPH_PATHS |= set(WIDE_ADMISSION_GRAPH)

BASE_CACHE_GRAPH = {
    "ci/managed-check/base_cache.py": (
        None,
        "dd852ef62365c5a1333f1d818ad84d539e19f4c41180be9ad97bdadcc0798970",
    ),
    "ci/managed-check/compare.py": (
        "7096d164997ca3ab224a0edc77003898190af33431cdc0640de1e26b9a5072cf",
        "33bcf90abbea57b28d61527a4af73d03c6ab2b0d3b16200578ad5289253c98e5",
    ),
    "ci/managed-check/bundle.sha256": (
        "d8ea452fadaace93fd7785d045aac330da534c80e9402ef2b26d20f7c9d55a24",
        "12b2491e2c66606e248986ec9b7a47670c4cf84f6f38db1b7e0c1e846a562bf5",
    ),
}

GRAPH_CHANGES += (BASE_CACHE_GRAPH,) + tuple(
    advance_graph(previous, BASE_CACHE_GRAPH)
    for previous in GRAPH_CHANGES
    if previous[BUNDLE][1] == BASE_CACHE_GRAPH[BUNDLE][0]
)
GRAPH_PATHS |= set(BASE_CACHE_GRAPH)

SOURCE_RECHECK_GRAPH = {
    "tools/gates/workflow.py": (
        "62d349176331acdab620a6c6154d9fab6495ee2491816cfdd87a996b0f77b826",
        "d31fc88e23c82e3c263af49d99a4f8f66d66e982e5396794c6947cdb7f085a72",
    ),
}
GRAPH_CHANGES += (SOURCE_RECHECK_GRAPH,)
GRAPH_PATHS |= set(SOURCE_RECHECK_GRAPH)

EMITTED_MSG_GRAPH = {
    "ci/managed-check/compare.py": (
        "0bc96c5aee42ed234c3600a9f726aba86c74763675c073d31161c1fa5adae969",
        "753b59e77e5338a51d6d3ee9897efaafa2e9e1633c5f3cfa913248a89f0bfb68",
    ),
    "ci/managed-check/bundle.sha256": (
        "1c7998cc3788b9efd66c714e7aff1fea6370bd698400fbda4629bdf209eba5b1",
        "a297c06e70fb86926416a15d5af6ac3b8ca2f31d3e3031614bd8c53c66b01195",
    ),
}
GRAPH_CHANGES = (SOURCE_RECHECK_GRAPH | EMITTED_MSG_GRAPH,) + GRAPH_CHANGES + (EMITTED_MSG_GRAPH,) + tuple(
    advance_graph(previous, EMITTED_MSG_GRAPH)
    for previous in GRAPH_CHANGES
    if BUNDLE in previous and previous[BUNDLE][1] == EMITTED_MSG_GRAPH[BUNDLE][0]
)
GRAPH_PATHS |= set(EMITTED_MSG_GRAPH)

EMITTED_ROWS_FIX_GRAPH = {
    "ci/managed-check/compare.py": (
        "753b59e77e5338a51d6d3ee9897efaafa2e9e1633c5f3cfa913248a89f0bfb68",
        "9b5ca88a540ed2a555944542dd16f9a4d3e3933e5a3a058a82f20362f5a79537",
    ),
    "ci/managed-check/wide.py": (
        "3f764d3df855f1d8aeed8bc05728667c3d376e9c2bd1a527cea196be434efefe",
        "618d17c6c5e8f83c64bf1c678a51f351f20a87087bbe2015c7ce0327cf6f3822",
    ),
    "ci/managed-check/bundle.sha256": (
        "a297c06e70fb86926416a15d5af6ac3b8ca2f31d3e3031614bd8c53c66b01195",
        "35c124efeef151a48c0cacc3a55125eedd52379ce0a1de23a76b76afd2ba59ee",
    ),
}
GRAPH_CHANGES += (EMITTED_ROWS_FIX_GRAPH,) + tuple(
    advance_graph(previous, EMITTED_ROWS_FIX_GRAPH)
    for previous in GRAPH_CHANGES
    if BUNDLE in previous and previous[BUNDLE][1] == EMITTED_ROWS_FIX_GRAPH[BUNDLE][0]
)
GRAPH_PATHS |= set(EMITTED_ROWS_FIX_GRAPH)

CUT12_GATES_AND_H2_GRAPH = {
    "tools/gates/config.json": (
        "2d0361d9efc1717ca387fb65359c60dc324b91be88d1ccafa347a172a9758b6d",
        "286aead9e860221789552e00f620dd45e7a70b2e033c95b6799b171edac6cd44",
    ),
    "tools/gates/rules.py": (
        "5eaf63471cbf7b08c03fdd72cc2fdb3f9e207a0a3b7790ccb966b61987994fc6",
        "abf702cf9409e67a51073cdb7bbf1851df6341b09868a9f4cc6ca7e1ed09022e",
    ),
    "tools/gates/test_gate.py": (
        "623e6723788683a7c37542b400ce5c70eae81effaca10fd9034d3a883fc49b47",
        "3bd32d9e59a6d51fbc43648bd93d360f9f47b407e2852ecdda25e407f4aee0f5",
    ),
    "tools/gates/test_public_delivery.py": (
        "5720e8ac40387613c01965da32ef4dbbb8dae731a2cf3fdbbe0e694ae41c1ded",
        "073710b7ff29eae5edeee8a3e7d67169387e44eadf06e1022443794c887e6e09",
    ),
    "ci/managed-check/header_producer.py": (
        "19106de010f61020ea4fcccf5db57a36969db037990deffd03881bc893e5b0c8",
        "11c41c57fb92ea42582545480e7da84f6de1b41f8642f0069c5abc97c1c5a9bc",
    ),
    "ci/managed-check/header_probe_plan.py": (
        "11e751e1a3b80aae3cd7909867357adc297628988b2b1e60065a3424ca928fe9",
        "53441502723614db269838f0bb5e203ea7f1d5409cf2553d5d48b67493eb0aa1",
    ),
    "ci/managed-check/bundle.sha256": (
        "35c124efeef151a48c0cacc3a55125eedd52379ce0a1de23a76b76afd2ba59ee",
        "80a8de235d4824307e47fd693296ff4097947e2133c48f72ec2730ab7acaf8b0",
    ),
}
GRAPH_CHANGES += (CUT12_GATES_AND_H2_GRAPH,) + tuple(
    advance_graph(previous, CUT12_GATES_AND_H2_GRAPH)
    for previous in GRAPH_CHANGES
    if BUNDLE in previous and previous[BUNDLE][1] == CUT12_GATES_AND_H2_GRAPH[BUNDLE][0]
)
GRAPH_PATHS |= set(CUT12_GATES_AND_H2_GRAPH)

CUT13_HEADER_TRUSTED_GRAPH = {
    "tools/gates/header_trusted.py": (
        None,
        "0320445ce074e7919add15245e5d45fcd4996137ed6af2bd930814bf0efe0b00",
    ),
    "tools/gates/header_source_preview.py": (
        "a09a7298441359f4f10303a2e8f012d0eed5f4bbde7f4a1623893f54c8d33ee3",
        "8100ed15bbb2594d884e89a905f47387d07ced2520616f15449ae440bff17ab1",
    ),
    "tools/gates/test_header_trusted.py": (
        None,
        "b014ebf68d8f463ad8cd134c178021dbebf59b8834a8e86f22bf126b2a2b5b44",
    ),
    "ci/managed-check/header_trusted_action.py": (
        None,
        "f5a167c0cce414c80e10e6cf3188d30028298b15ff805c2592ad42117d926565",
    ),
    "ci/managed-check/bundle.sha256": (
        "80a8de235d4824307e47fd693296ff4097947e2133c48f72ec2730ab7acaf8b0",
        "af8387224c9d53242728b6e4b70aa1ca97567670c00b6a8a3a2527cd1a28f49a",
    ),
}
GRAPH_CHANGES += (CUT13_HEADER_TRUSTED_GRAPH,) + tuple(
    advance_graph(previous, CUT13_HEADER_TRUSTED_GRAPH)
    for previous in GRAPH_CHANGES
    if BUNDLE in previous and previous[BUNDLE][1] == CUT13_HEADER_TRUSTED_GRAPH[BUNDLE][0]
)
GRAPH_PATHS |= set(CUT13_HEADER_TRUSTED_GRAPH)

CUT14_TOOLS_AND_H5_GRAPH = {
    'tools/gates/config.json': ('286aead9e860221789552e00f620dd45e7a70b2e033c95b6799b171edac6cd44', 'b712b72efba3b027e6bc7d99a6da75c9decc100e78b28c9e385000082a64f2ce'),
    'tools/gates/rules.py': ('abf702cf9409e67a51073cdb7bbf1851df6341b09868a9f4cc6ca7e1ed09022e', '6d404b8cea627498d7e36288f7c0f806b7debf1ec988eab1cec8d0f82e966459'),
    'tools/gates/test_gate.py': ('3bd32d9e59a6d51fbc43648bd93d360f9f47b407e2852ecdda25e407f4aee0f5', 'd58da420612a4759917f742d7a8f9d97ff375e56594cf2d967ffce6c9806a8d0'),
    'tools/gates/test_public_delivery.py': ('073710b7ff29eae5edeee8a3e7d67169387e44eadf06e1022443794c887e6e09', '5f74b7283f335e2f78bc5a74a669ab70f0aab051c9c1dbb4fa63d93741fcb30e'),
    'ci/header-witness/h5_layout.py': ('4f03c1aeb9d33aa7a20e47e75e38cf8c4f3b20aebaf6fc5ddd58034a33e06780', 'a16e9682d8cee522682c6ab64d48ee0452b44c6d9a50ff5e8ce178b45169447e'),
    'ci/header-witness/h5_witness.py': ('dbd0d86bb8ede7ff597f99f12ead6c3238b68a0cbba4594e8a1a6655bf0ff36b', '22e1c8e611137c2eab29cb4b09e1b7cba9a0044c8f507d34e8e7f5c96bdba586'),
    'ci/header-witness/h5_evidence.py': ('13083a5505b0413a2857ce32d2b59b2dc2a8b2eccad8f7c3a1207f3191a024a5', '8854d1cfb797066b631266bfcc060572f23e4173b091b8391bc0313623998b93'),
    'ci/header-witness/h5_bridge.py': ('e746dd464782f148f0316ba6b316abb7a10e24f8ae1f667cd617bd70c852cf22', '0c96ae93edce25dcff3d23cefb3603ffacc96b1ebf8ee13025dd69063b75deb8'),
    'ci/header-witness/bundle.sha256': ('0ab62cc857cb9c52410261e82994bf74035873e1fbdc0233bbe07de1616a022f', '3b3aa6717248c971ddd903f30b71ddcb160c552d959bf98df669ca2f6928ebb2'),
    'ci/header-witness/tests/test_h5_layout.py': ('387fa6f62e34f70a24a1698ef866fceb36c0e1089d5297213ec82368ac250984', '330c0a79578291a6b8d45d01c56054ab9d90f668878bdaa84649ad8c6a2c7d9e'),
    'ci/header-witness/tests/test_h5_bridge.py': ('9307061c71645bbe4b936067dfe43fb42fb7474a41b3f401eb743e123092f663', 'e4f7dffe177a9df1da7ac24108c4691523cfca6a03a67c54151cd34acdb363b7'),
}
GRAPH_CHANGES += (CUT14_TOOLS_AND_H5_GRAPH,) + tuple(
    advance_graph(previous, CUT14_TOOLS_AND_H5_GRAPH)
    for previous in GRAPH_CHANGES
    if BUNDLE in previous and previous[BUNDLE][1] == CUT13_HEADER_TRUSTED_GRAPH[BUNDLE][1]
    and all(previous[path][1] == CUT14_TOOLS_AND_H5_GRAPH[path][0]
            for path in previous.keys() & CUT14_TOOLS_AND_H5_GRAPH.keys())
)
GRAPH_PATHS |= set(CUT14_TOOLS_AND_H5_GRAPH)

CUT15_H5_VSLOT_GRAPH = {
    'ci/header-witness/h5_layout.py': ('a16e9682d8cee522682c6ab64d48ee0452b44c6d9a50ff5e8ce178b45169447e', '970efd403ac11fdeb13a208c48b15dc59481436c0697ecaf1faefc309f8b75fa'),
    'ci/header-witness/h5_evidence.py': ('8854d1cfb797066b631266bfcc060572f23e4173b091b8391bc0313623998b93', 'a2a92d5551cc46fc1bfc9f6c37669d31f59fc437dba18bfb10843f83f825bcf8'),
    'ci/header-witness/bundle.sha256': ('3b3aa6717248c971ddd903f30b71ddcb160c552d959bf98df669ca2f6928ebb2', '1b9db91931d7a2de32e6f6280a41fae2fcdb4dde47488446adf07603dedc7443'),
    'ci/header-witness/tests/test_h5_layout.py': ('330c0a79578291a6b8d45d01c56054ab9d90f668878bdaa84649ad8c6a2c7d9e', '1d529e070a992a7d2efa1ba5e645d3d41fbcf9a896a93b25c8252ddd09328865'),
}
GRAPH_CHANGES += (CUT15_H5_VSLOT_GRAPH,) + tuple(
    advance_graph(previous, CUT15_H5_VSLOT_GRAPH)
    for previous in GRAPH_CHANGES
    if (previous.keys() & CUT15_H5_VSLOT_GRAPH.keys())
    and all(previous[path][1] == CUT15_H5_VSLOT_GRAPH[path][0]
            for path in previous.keys() & CUT15_H5_VSLOT_GRAPH.keys())
)
GRAPH_PATHS |= set(CUT15_H5_VSLOT_GRAPH)

CUT16_GATES_DXWRAPPER_GRAPH = {
    'tools/gates/config.json': ('b712b72efba3b027e6bc7d99a6da75c9decc100e78b28c9e385000082a64f2ce', '8efc59a0cd826377dc8b95515b6c7dfcbac9158e5bf7c03d3fa86a60f393c5c9'),
    'tools/gates/test_public_delivery.py': ('5f74b7283f335e2f78bc5a74a669ab70f0aab051c9c1dbb4fa63d93741fcb30e', 'a820399cfb7dbb644d50685605146c9b3ef11e08a2664c1edc90bee6d1211788'),
}
GRAPH_CHANGES += (
    CUT16_GATES_DXWRAPPER_GRAPH,
    advance_graph(CUT15_H5_VSLOT_GRAPH, CUT16_GATES_DXWRAPPER_GRAPH),
) + tuple(
    advance_graph(previous, CUT16_GATES_DXWRAPPER_GRAPH)
    for previous in GRAPH_CHANGES
    if (previous.keys() & CUT16_GATES_DXWRAPPER_GRAPH.keys())
    and all(previous[path][1] == CUT16_GATES_DXWRAPPER_GRAPH[path][0]
            for path in previous.keys() & CUT16_GATES_DXWRAPPER_GRAPH.keys())
)
GRAPH_PATHS |= set(CUT16_GATES_DXWRAPPER_GRAPH)

DISJOINT_PUBLISH_GRAPH = {
    'ci/managed-check/github_io.py': ('c34a1c9bde3d83f39346bdb8246ee74290cebd5058672d9e24b13388bb02c9e8', '5fd7f45a9af086ad83f19ed1fa4925a041da85de8466a5c621e0a5e0554fe754'),
    'ci/managed-check/github_status.py': ('4e54773993a7868b6a6d20d198e12b51babb7cdde52880f7ab50949e4d620eaf', '0df044f72234d6366819ddfddce407bffaf439aa1c4d82fd400a8a085997ccb1'),
    'ci/managed-check/inline_status.py': ('c8b03c2bbf2a9ed3b6fa21b4b30fb84de6d7b522c24975d5f70aaf71b305adbf', '61efa6f732bbf7dbc736492e37b9ab37411274fefd046ce87def8295131d974f'),
    'ci/managed-check/bundle.sha256': ('af8387224c9d53242728b6e4b70aa1ca97567670c00b6a8a3a2527cd1a28f49a', 'db134457bdbb68b1aed9571f374e1d829e4f9abf7e314994f847ce49966538d8'),
}
GRAPH_CHANGES += (DISJOINT_PUBLISH_GRAPH,) + tuple(
    advance_graph(previous, DISJOINT_PUBLISH_GRAPH)
    for previous in GRAPH_CHANGES
    if BUNDLE in previous and previous[BUNDLE][1] == DISJOINT_PUBLISH_GRAPH[BUNDLE][0]
    and all(previous[path][1] == DISJOINT_PUBLISH_GRAPH[path][0]
            for path in previous.keys() & DISJOINT_PUBLISH_GRAPH.keys())
)
GRAPH_PATHS |= set(DISJOINT_PUBLISH_GRAPH)

CUT17_LOCALFILE_EH_GRAPH = {
    'ci/managed-check/compare.py': ('9b5ca88a540ed2a555944542dd16f9a4d3e3933e5a3a058a82f20362f5a79537', '9161ea0a99eb48ef2ecd899136c2915136f323aa5ed67838216610d104232599'),
    'ci/managed-check/emitted_data.py': ('2ac0335a3f5836b30f55671f72cb244eedb64bb4ebba08dd2cb38a1b3aa57658', 'fbd0ed8904dc549247687aa800970edbde08aa2eff85c8a3f6d81cd152674002'),
    'ci/managed-check/bundle.sha256': ('b39102634ec516e37f0c04317c19f28d8bd7733aa327ad514ebf4688cb320646', '2ffd86ae19b71543207377ce0592a7b10c27c674546bdea71b26c26344af711e'),
}
GRAPH_CHANGES += (CUT17_LOCALFILE_EH_GRAPH,) + tuple(
    advance_graph(previous, CUT17_LOCALFILE_EH_GRAPH)
    for previous in GRAPH_CHANGES
    if BUNDLE in previous and previous[BUNDLE][1] == CUT17_LOCALFILE_EH_GRAPH[BUNDLE][0]
    and all(previous[path][1] == CUT17_LOCALFILE_EH_GRAPH[path][0]
            for path in previous.keys() & CUT17_LOCALFILE_EH_GRAPH.keys())
)
GRAPH_PATHS |= set(CUT17_LOCALFILE_EH_GRAPH)

TRUSTED_WIDE_ADMISSION_GRAPH = {
    'ci/managed-check/fork_admission.py': ('25badb095dabc6c8a717145f3ea1e14342fbb5982fb10982608e9df7b8e01e34', 'f96323dad8d71a945b74b4eb33dec0ca860e3388d7af8a9d75ae78a53389e36e'),
    'ci/managed-check/bundle.sha256': ('2ffd86ae19b71543207377ce0592a7b10c27c674546bdea71b26c26344af711e', 'c5d8fb205e785853cf3bfe36147852b17e750b20764fb5d5e2164f4b57aee4d0'),
}
GRAPH_CHANGES += (TRUSTED_WIDE_ADMISSION_GRAPH,) + tuple(
    advance_graph(previous, TRUSTED_WIDE_ADMISSION_GRAPH)
    for previous in GRAPH_CHANGES
    if BUNDLE in previous and previous[BUNDLE][1] == TRUSTED_WIDE_ADMISSION_GRAPH[BUNDLE][0]
    and all(previous[path][1] == TRUSTED_WIDE_ADMISSION_GRAPH[path][0]
            for path in previous.keys() & TRUSTED_WIDE_ADMISSION_GRAPH.keys())
)
GRAPH_PATHS |= set(TRUSTED_WIDE_ADMISSION_GRAPH)



# Exact pinned TU-move migrations: carry untouched TUs across a maintainer TU relocation while
# forcing every moved TU to be freshly compiled and verified at its new path.
# `new_tus` require absent-at-parent and present-at-main (any regular blob) so a follow-up edit
# to a relocated TU before state publishes remains incremental instead of stranding state.
CUT14_TOOLS_TU_MOVE = {
    "old_tus": frozenset({
        "src/Code/Tools/WW3D/max2w3d/bchannel.cpp",
        "src/Code/Tools/WW3D/pluglib/WWmatrix3.cpp",
        "src/Code/Tools/WW3D/pluglib/matrix4.cpp",
    }),
    "new_tus": frozenset({
        "src/unk/O00160110.cpp",
        "src/unk/O00149310.cpp",
        "src/unk/O00133510.cpp",
    }),
    "files": {
        'config/map.json': ('d0e52437b4c09c6fb312e200482796ab9c0a7fe36100231f0a9fa4b645fa0688', '7a8856757c5332f522873643d8fae134616dc39a918f0f6156144fa8d7831bd0'),
        'config/reseed21.json': ('0779a9dffaf972fe2fa5e90e0c67a56140e9e5e352dafc06c38e40e7b34a07a3', 'e6149d31b5942dee55ba6e48847d7081d5a658c87bdb9b80e763295612b1c604'),
        'src/Code/Tools/WW3D/max2w3d/bchannel.cpp': ('87bf709550d65ccdb54ab10413da44af719fc934c7ce6a82ef7463a1c8fe68cf', None),
        'src/Code/Tools/WW3D/pluglib/WWmatrix3.cpp': ('f99c7e56d0f9401b882f4c3b26a40806413e80bc7b0516bfce3545509d8a03d2', None),
        'src/Code/Tools/WW3D/pluglib/matrix4.cpp': ('fbf266aff03e0090021f675108fa2ef71a05e1649e4db4afc7187a3bcf3c31e0', None),
        'expected/Code/Tools/WW3D/max2w3d/bchannel.cpp.json': ('72021818bc36b4df2b430dddddf6312225fa2f172cc3d26366d55193bf989a98', None),
        'expected/Code/Tools/WW3D/pluglib/WWmatrix3.cpp.json': ('bc2b76aaa7722695d1a27fa708893abaaea0139ba6a5f6aaf3aa898d49747784', None),
        'expected/Code/Tools/WW3D/pluglib/matrix4.cpp.json': ('8f507a1fdec5aac325dda68d9fb6ff705c14b17e698dfe4be87b0e5ffea6cdd5', None),
        'expected/Code/Tools/WW3D/max2w3d/bchannel.cpp.neighbours.json': ('f94ddb28a675fea366697bf3ce5a99d7f85a0e83e237753c4e943689ca04fb98', None),
        'expected/Code/Tools/WW3D/pluglib/WWmatrix3.cpp.neighbours.json': ('c4bf57c1c3346277e82726a0b77fe08a23821c73305806da2ca1884493665bb8', None),
        'expected/Code/Tools/WW3D/pluglib/matrix4.cpp.neighbours.json': ('5fdfa6fe0f0c39187d25e2cf148b3243d574719fa862bb7bb5e3c21caecd257b', None),
        'expected/unk/O00160110.cpp.json': (None, '72021818bc36b4df2b430dddddf6312225fa2f172cc3d26366d55193bf989a98'),
        'expected/unk/O00149310.cpp.json': (None, 'bc2b76aaa7722695d1a27fa708893abaaea0139ba6a5f6aaf3aa898d49747784'),
        'expected/unk/O00133510.cpp.json': (None, '8f507a1fdec5aac325dda68d9fb6ff705c14b17e698dfe4be87b0e5ffea6cdd5'),
        'expected/unk/O00160110.cpp.neighbours.json': (None, 'f94ddb28a675fea366697bf3ce5a99d7f85a0e83e237753c4e943689ca04fb98'),
        'expected/unk/O00149310.cpp.neighbours.json': (None, 'c4bf57c1c3346277e82726a0b77fe08a23821c73305806da2ca1884493665bb8'),
        'expected/unk/O00133510.cpp.neighbours.json': (None, '5fdfa6fe0f0c39187d25e2cf148b3243d574719fa862bb7bb5e3c21caecd257b'),
    },
}
CUT16_WB_RENAMES = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        'include/ww3d2/dx8wrapper.h',
        'include/ww3d2/dxwrapper.h',
        'src/code/gameenginedevice/source/milesaudiodevice/milesstreamhandlewrapperimplementation.cpp',
        'src/code/gameenginedevice/source/w3ddevice/gameclient/baseheightmap.cpp',
        'src/code/gameenginedevice/source/w3ddevice/gameclient/w3dshadermanager.cpp',
        'src/code/gameenginedevice/source/w3ddevice/gameclient/w3dsmudge.cpp',
        'src/code/gameenginedevice/source/w3ddevice/gameclient/w3dtreebuffer.cpp',
        'src/code/libraries/source/wwvegas/ww3d2/camera.cpp',
        'src/code/libraries/source/wwvegas/ww3d2/dxwrapper.cpp',
        'src/code/libraries/source/wwvegas/ww3d2/fxshadernamespacesas.cpp',
        'src/code/libraries/source/wwvegas/ww3d2/mapper.cpp',
        'src/code/libraries/source/wwvegas/ww3d2/sortingrenderer.cpp',
        'src/code/libraries/source/wwvegas/ww3d2/ww3d.cpp',
        'src/code/libraries/source/wwvegas/wwmath/matrix4.cpp',
        'src/code/libraries/source/wwvegas/wwmath/quat.cpp',
        'src/code/libraries/source/wwvegas/wwmath/vp.cpp',
        'src/unk/g00042f57.cpp',
        'src/unk/g0006607a.cpp',
        'src/unk/g0006e25e.cpp',
        'src/unk/g00075295.cpp',
        'src/unk/g00083b9e.cpp',
        'src/unk/g001101d7.cpp',
        'src/unk/g00133730.cpp',
        'src/unk/g00714d80.cpp',
        'src/unk/o0006615f.cpp',
        'src/unk/o00066322.cpp',
        'src/unk/o0006e18d.cpp',
        'src/unk/o0006e1c0.cpp',
        'src/unk/o00075343.cpp',
        'src/unk/o00083b81.cpp',
        'src/unk/o0010713a.cpp',
        'src/unk/o0010737e.cpp',
        'src/unk/o001100e8.cpp',
        'src/unk/o0011c470.cpp',
        'src/unk/o00126071.cpp',
        'src/unk/o00126082.cpp',
        'src/unk/o0012639e.cpp',
        'src/unk/o0012d2b0.cpp',
        'src/unk/o0012d2c0.cpp',
        'src/unk/o0012e010.cpp',
        'src/unk/o00133510.cpp',
        'src/unk/o00139290.cpp',
        'src/unk/o0013c930.cpp',
        'src/unk/o007166d0.cpp',
    }),
    "files": {
        'config/asm_allowlist.json': ('cfd34ee394cdaa83502fed0f0f37ed3cde92b710c7f6c0385d6c8e66b63b7ba2', 'f22c5c29215d721e88c3c0483a4e2c0d1d80c9f2cf3a64e7834aa6d70cc9a1dc'),
        'config/demangled.json': ('3a40e3c7b5db9d2980037d89b541a4a292e0e60278f1289ad041226e0babf2ac', '25f6504bab2aa06472efbac9e9b46d194de564b6108ed5c0a57fe188be2785bf'),
        'config/header_manifest.json': ('88f306fc21826a2192209354e6cf0cc6841446f842d60e5ae8b814e16ccec78c', 'e292e7871dd8b8eb6eb547aa063df79d39fe3d1c8e6f546b1b8f0a21daba3c80'),
        'config/identity_overlays.json': ('ea6196242d98ffcd162acb0883db643e55b66b27bce85024e6c3c43db1cef18a', '2c106263c5cf7538d3573dd74736323582d53c1fe71909c4cf49ce125c400f4c'),
        'config/map.json': ('7a8856757c5332f522873643d8fae134616dc39a918f0f6156144fa8d7831bd0', 'e4a674cf0243309b957b1b75d738b35c4c68f603d1fd50d6755fdfa8642aa3bb'),
        'config/symbols.json': ('8cb23eb010238ed89bd2d05ebf98dccf84111f4fdbe4f0a858860573637319d9', 'd3580b3b46546f6c5f2e738d6c03edfd570d24f1b1289a2345597dfd3ac43b02'),
        'expected/Code/GameEngineDevice/Source/MilesAudioDevice/MilesStreamHandleWrapperImplementation.cpp.json': ('0599e6022d546e57d1eb69be92a84293376e8a17d409e1e237cde2d98ab2fc83', 'a9b8efb817d7c36088e792f67153ef469ea620218dc9c994ea339f32736145fa'),
        'expected/Code/GameEngineDevice/Source/W3DDevice/GameClient/BaseHeightMap.cpp.json': ('c5eb6a49b06dcdf62ce8bea5c9e0d636c3eb6e4b0511abcdb1f50a9d7d003c7d', 'ace5f5707c41798de084994847fe8bb9e7c687a380abf5369723f3a28de727dc'),
        'expected/Code/GameEngineDevice/Source/W3DDevice/GameClient/W3DShaderManager.cpp.json': ('c211325388cf2abdecdf796acc6cdff028ee5223372f28770df04a3023c779ef', '99472e8be40100a1d7d3d356fac7165386a07859d413f9c3041d980d1ee699e8'),
        'expected/Code/GameEngineDevice/Source/W3DDevice/GameClient/W3DSmudge.cpp.json': ('ac395e003017a8af4983fbfc756acb976fc929637af14a9ba7ce6b6b31c0f679', '10a0747df5106cd06bfa91e81525e1baf0b60cbd730bd64c0d0238e0255b237d'),
        'expected/Code/Libraries/Source/WWVegas/WW3D2/dxwrapper.cpp.json': ('58820d6f5ca9c56795a6665fb92e7c8e8232a7a2c805035b8c81faa2eccdb36d', 'b17adf428e5aceff1eb1f2816e3fe850ca01f10379b2d9e1bd8848e8506a0e82'),
        'expected/Code/Libraries/Source/WWVegas/WW3D2/fxshadernamespacesas.cpp.json': ('d1f855f6950b5f9a8e5fb92516caad6aac2b0d3fd22efd5af57baa4280fbea61', 'c83fe5010396ff62bb7e065546e5acaeda0c627f11c6e9044da700d119ebc2fb'),
        'expected/Code/Libraries/Source/WWVegas/WW3D2/mapper.cpp.json': ('4201a8d46701bb085a7b30a958b8a87efa531becf8925c594a8ad2fddc079530', '81821a47d5ebe8444781ac287ccb1bfd108f94fe17a0481ed75dfc21cb067d42'),
        'expected/Code/Libraries/Source/WWVegas/WW3D2/sortingrenderer.cpp.json': ('426e8806d137f0aca30d6e5dd271e014d627ee3b08b135b401cd4828a5b59013', '83720fb66a89b8f390f4ee45c793f013461803ad80ed833555ab4b240bbb0d62'),
        'expected/Code/Libraries/Source/WWVegas/WW3D2/ww3d.cpp.json': ('9ee25f4e513244b5c026283eff741c3fb3a39881ea5feae97eccc765526ab84b', 'e5951b556268f920d9b53c0c4f65cf4fe00088b0011e8b0177489ef30c0a9079'),
        'expected/unk/O0006615F.cpp.json': ('852c1c19ecf4d95224fff4bcb9a876530e0b90b5af17ccd44e21d72bb0383eed', 'dddbc583015da8d0c7b7b70d33ff6d37ae970a7aa26a9f03640a3c5658ac9572'),
        'expected/unk/O0010737E.cpp.json': ('bdf349b1267503eb2e32fad3296f22c988626600b7364e6c7262f44528ba6352', '2f8a37cd7889a57770ef1d5d45d88ce2147cb384829717009788b306fecc57ce'),
        'expected/unk/O0012639E.cpp.json': ('26ab688132c953a19fc1e133abdeb5fe2c5ba34d705dd10c847f900be3f6cb66', '913a2b27668cd454efba6abcc353eb020717b32263d753606da13033551822b1'),
        'include/WW3D2/DX8Wrapper.h': ('c2af13615d3c69c24005e655c87d35bd78c19882e996db7d0b9e3f2e9c95ee0b', None),
        'include/WW3D2/dxwrapper.h': (None, '30e51d9c3a92a2570a67af873da9a754c78c3e6dd88ecb75e9b57ab03a844053'),
    },
}
ISSUE541_PROFILES_AND_GAP_UNITS = {
    "old_tus": frozenset({
        "src/unk/G00492E43.cpp",
        "src/unk/O00492EFD.cpp",
        "src/unk/G004C636B.cpp",
    }),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "include/gamelogic/module/siegedeployhordespecialpower.h",
        "include/nbench/emfloat.h",
        "src/nbench/benchmark/emfloat.c",
        "src/unk/g006dfdf0.cpp",
        "src/unk/o00492c8a.cpp",
        "src/unk/o004c63d8.cpp",
    }),
    "files": {
        'config/map.json': ('e4a674cf0243309b957b1b75d738b35c4c68f603d1fd50d6755fdfa8642aa3bb', '7adb5658117df6d75656f6138db4fa08cf68784707f25cc2c4b4f823c792f34e'),
        'expected/unk/G00492E43.cpp.json': ('76fcce5616f03b2d9c5349794cdf6bad0a43f1d6a18c55a61f2cbeaca385b4e3', None),
        'expected/unk/G004C636B.cpp.json': ('7a96aa4942f44018ddc3b466ef29bcc88bbee2cf12d1761a78e96022f85dee14', None),
        'expected/unk/O00492C8A.cpp.json': ('52d2bd49505847066c2275c0f4d177ad83e0a42b0d8e96ecb9cc12fe03043bff', 'e5132ffcd97cef0a0dd8fa1868d9ad82b0610df927168e62a71e3f0e48e4d4d4'),
        'expected/unk/O00492EFD.cpp.json': ('7181e4c8342b804d4dc9abaa83ab5d2a01a11735ecaa3545adb22aaadb338578', None),
        'expected/unk/O00492EFD.cpp.neighbours.json': ('a6393fdac86b3ab6d8bd64d6a05dc15c6b820623899bfe8bc9351ed5a2cf8855', None),
        'include/nbench/emfloat.h': (None, 'da856cf83adb73aa0f487ea523469fede1a41be0adb88b2dfe4897b1c18a9921'),
        'src/unk/G00492E43.cpp': ('ba9c9ae6524135e7f80e23e1ad64996cecc9c1f8fbd606723dd27b8776a463f9', None),
        'src/unk/G004C636B.cpp': ('27fea822c92fff0a73e6bf73fb100e410698842c8b12e59075d6284123cac595', None),
        'src/unk/O00492EFD.cpp': ('2d2fbd752d9f27d052709f1b492547861210d43bbd906f5eb4d825410fe8e607', None),
    },
}
CUT17_LOCALFILE_LEDGER = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "include/common/localfile.h",
        "src/code/gameengine/source/common/system/localfile.cpp",
        "src/code/libraries/source/file/streamingarchivefile.cpp",
    }),
    "files": {
        "config/demangled.json": ("25f6504bab2aa06472efbac9e9b46d194de564b6108ed5c0a57fe188be2785bf", "88a57a2021784998def670e58f8f5c730216e1b15782df00985fd4e4792ee3a7"),
        "config/map.json": ("7adb5658117df6d75656f6138db4fa08cf68784707f25cc2c4b4f823c792f34e", "db50153057d80dba9ecfe3b9852910e97d555ba53a5e72c5144e9c5bafcb00b2"),
        "config/symbols.json": ("d3580b3b46546f6c5f2e738d6c03edfd570d24f1b1289a2345597dfd3ac43b02", "c22f1058cd03c0259958aaa859d0f00ea3edf41cd4fdf74c3ad3d3a194c243ce"),
        "expected/Code/Libraries/Source/file/StreamingArchiveFile.cpp.json": ("c1ab30e566745ca7780120cdb4be7a869ce5b156e57e9e53a0ffbc65450699d3", "ae08752a125eca13b69fa231777f7caeaca4d5a3cb0f990684ebbfcef8db8bec"),
    },
}
ISSUE541_AND_CUT17_LOCALFILE_LEDGER = {
    "old_tus": ISSUE541_PROFILES_AND_GAP_UNITS["old_tus"] | CUT17_LOCALFILE_LEDGER["old_tus"],
    "new_tus": ISSUE541_PROFILES_AND_GAP_UNITS["new_tus"] | CUT17_LOCALFILE_LEDGER["new_tus"],
    "force_changed": ISSUE541_PROFILES_AND_GAP_UNITS["force_changed"] | CUT17_LOCALFILE_LEDGER["force_changed"],
    "files": advance_graph(ISSUE541_PROFILES_AND_GAP_UNITS["files"], CUT17_LOCALFILE_LEDGER["files"]),
}
ISSUE20_HEADERS_AND_LEDGER = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "include/common/gamestate.h",
        "include/common/watchdog.h",
        "include/draft/aptradialmenu/impl/buttonshell.h",
        "include/draft/rva000e3b41.h",
        "include/draft/rva0053ed1a.h",
        "include/draft/rva00577f77.h",
        "include/draft/watchdog.h",
        "include/draft/weathersetting.h",
        "include/draft/worldheightmap.h",
        "include/gameclient/aptradialmenu.h",
        "include/gameclient/baseheightmaprenderobjclass.h",
        "include/gameclient/flatheightmap.h",
        "include/gameclient/gloweffect.h",
        "include/gameclient/particlesys.h",
        "include/gameclient/radialmenulayoutcontroller.h",
        "include/gameclient/radialwindowcontroller.h",
        "include/gameclient/smoothcameraheightfield.h",
        "include/gameclient/snowmanager.h",
        "include/gameclient/w3dprojectedshadow.h",
        "include/gameclient/w3dprojectedshadowmanager.h",
        "include/gameclient/w3dterrainbackground.h",
        "include/gameclient/w3dterraintracks.h",
        "include/gameclient/w3dview.h",
        "include/gameclient/weathersetting.h",
        "include/gameclient/worldheightmap.h",
        "include/gamelogic/bridge.h",
        "include/ww3d2/dx8cleanuphook.h",
        "include/ww3d2/dxwrapper.h",
        "include/ww3d2/shadow.h",
        "include/wwmath/aaboxclass.h",
        "src/code/gameengine/source/common/gameengine.cpp",
        "src/code/gameengine/source/common/system/watchdogthread.cpp",
        "src/code/gameengine/source/gamelogic/map/terrainlogic.cpp",
        "src/code/gameengine/source/gamenetwork/gameinfo.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/shadow/w3dprojectedshadow.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3dterrainvisual.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3dview.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/water/w3dwatertracks.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/worldheightmap.cpp",
        "src/unk/g00062a52.cpp",
        "src/unk/g00309df6.cpp",
        "src/unk/g0030e67c.cpp",
        "src/unk/o000f0e6c.cpp",
        "src/unk/o00311431.cpp",
    }),
    "files": {
        "config/demangled.json": ('88a57a2021784998def670e58f8f5c730216e1b15782df00985fd4e4792ee3a7', 'ec5ca7a2f65536341ca308ed2bd4817c3d86dfe56d2e5fcec5327a114c54221b'),
        "config/map.json": ('db50153057d80dba9ecfe3b9852910e97d555ba53a5e72c5144e9c5bafcb00b2', '2a6b32557a297ac080f0578802e384a9c5552df305931d1882e9a5ae8ac04fe8'),
        "expected/Code/GameEngine/Source/Common/GameEngine.cpp.json": ('4b21e003b2f578584eb3fc7a2e18902f1541f63143e3a036945b832b847dc9b0', '812383323e55b546558ea21cb0d414399848c299c945b596f7b4d5d5aa48965e'),
        "expected/Code/GameEngine/Source/GameNetwork/GameInfo.cpp.json": ('7ab4133f6bffe8f196456ed7c505a03b8b41575b40ebd068e5f3466e93c95bbb', 'b518c2ae9d9f91ab302eef7068b7dc71b0a3d618196ab86b6df7ea95b5fe7012'),
        "expected/Code/GameEngineDevice/Source/W3DDevice/GameClient/Shadow/W3DProjectedShadow.cpp.json": ('434d2cbb614a4e0248f885ca11d75b7c5111ddb1e3e87be4e54dbc328ef8ca7f', '02d8a39bb28dd8e06e7a4aac4090632c1d60b2ad17462b08eb45696ecdac847d'),
        "expected/Code/GameEngineDevice/Source/W3DDevice/GameClient/W3DView.cpp.json": ('716b1fe9c4c53f2763e2e02447e2649bd9bca32f2b8b0bad85d88fa6abe0f8bc', 'f8c28678151a2cb673b56167f2b20a019334e742e67690bb1e9ae1eb3d743300'),
        "expected/unk/O000F0E6C.cpp.json": ('9ac8bc3d06a1b02b24df9a09bb69dc384f1a5c3893f03355bc4cba4ea23f239a', 'c13ba120354c0d051800194d7dd777b49ed58fe1077ab77144329961fb552921'),
        "expected/unk/O00311431.cpp.json": ('dc8bac813c40d168ea0c1f6bbbaa9e09c6099b15214fb6298d725d31ce558fb2', '4e5b4adfb06cb397f470dbf78c773f6eea0ab3154c71e4a14ec49e42c153a4c7'),
        "include/Common/Watchdog.h": (None, '797ccdfcc06742f9594308813b63e688bd74b840c7b8aa61c02c71081b58729d'),
        "include/Draft/AptRadialMenu/Impl/ButtonShell.h": ('d09e12af32bac0afd1ae0a8dc6cc0dd8c07805056570968312486b2d2c76cf3e', None),
        "include/Draft/Rva000E3B41.h": ('bad535db238aa5907068632439d86f6ef3ae2c1a9d173254a613f8b734dbbbbe', None),
        "include/Draft/Rva0053ED1A.h": ('912b785195f574afd0cbb00e917ff6f22048ba39fd8b04f5ccf9fb6f759c57ab', None),
        "include/Draft/Rva00577F77.h": ('aa7ba565ba9350903b1c7b3d70899ccf54e7e6ce12c965bc9822501ffb933665', None),
        "include/Draft/Watchdog.h": ('19dfe94affcf6b3d0f3ff4ca0adc7e92df0fec3034eefaa630af107c43006a0f', None),
        "include/Draft/WeatherSetting.h": ('fb9c84ae4c698ef8f63cc53464c27e2b7b64a6f71f33cfc92cb980c02f2a2121', None),
        "include/Draft/WorldHeightMap.h": ('fc790b004a5cb0adb6475bd2f1e4ee5e8584c50e29b64c6099138bb35948352f', None),
        "include/GameClient/AptRadialMenu.h": (None, 'd2e444fe6845c1dffeb1b999fbb5d5a8170cbc79f24ca9d71f3ca614a06c1c18'),
        "include/GameClient/FlatHeightMap.h": (None, '91007b7e58ef14093c99109d8d1710946a8b784ee8e2ecda45b2411d9d92fe5f'),
        "include/GameClient/GlowEffect.h": (None, '58fab7220b6ab6bef141efb272df31e363190975f08dd46e68159a9345963fe3'),
        "include/GameClient/RadialMenuLayoutController.h": (None, '52051e2199b105a4c7f30f729747781d6d0713f2ad2f953837f65954f99d51ff'),
        "include/GameClient/RadialWindowController.h": (None, '662970cbd209eccd22fb29ce9cff4a105cb818c8d2efcb53ac1db5ee6a506db6'),
        "include/GameClient/SmoothCameraHeightField.h": (None, '50c3c16284cd4dfc98c30e417eec7467fd0188df61165f7ad6fe635286ff032d'),
        "include/GameClient/W3DProjectedShadow.h": (None, 'cac4a20aa817dc3482d533fdb9bf64bc527ec20325ffa9d76f7ca7403be00bbc'),
        "include/GameClient/W3DTerrainBackground.h": (None, 'd96ab6c37780dd88cf1aacd234e23378a9dfe57e6cc57f876f231261131b1e36'),
        "include/GameClient/W3DTerrainTracks.h": (None, '30d38cc8e4ecebea0aef5e418bd5342e545c8e08ab23f99aaa3f8b652c23bb2a'),
        "include/GameClient/WeatherSetting.h": (None, '0e4b0e3519e00d8533f85e064a2ef72023f586cabba5995a90e6fbd0cd837249'),
        "include/GameClient/WorldHeightMap.h": (None, 'eb6ea748e21085aaae75f443643fa8fb2f478bee9e3794cc7de554a637dd6fbe'),
        "include/WW3D2/DX8CleanupHook.h": (None, 'e9c0de63b4f1777ce985079b6e7e95c885eebc51be6576dbd1f7f498ac2a2f39'),
    },
}
CUT17_AND_ISSUE20_LEDGER = {
    "old_tus": CUT17_LOCALFILE_LEDGER["old_tus"] | ISSUE20_HEADERS_AND_LEDGER["old_tus"],
    "new_tus": CUT17_LOCALFILE_LEDGER["new_tus"] | ISSUE20_HEADERS_AND_LEDGER["new_tus"],
    "force_changed": CUT17_LOCALFILE_LEDGER["force_changed"] | ISSUE20_HEADERS_AND_LEDGER["force_changed"],
    "files": advance_graph(CUT17_LOCALFILE_LEDGER["files"], ISSUE20_HEADERS_AND_LEDGER["files"]),
}
ISSUE541_CUT17_AND_ISSUE20_LEDGER = {
    "old_tus": ISSUE541_AND_CUT17_LOCALFILE_LEDGER["old_tus"] | ISSUE20_HEADERS_AND_LEDGER["old_tus"],
    "new_tus": ISSUE541_AND_CUT17_LOCALFILE_LEDGER["new_tus"] | ISSUE20_HEADERS_AND_LEDGER["new_tus"],
    "force_changed": ISSUE541_AND_CUT17_LOCALFILE_LEDGER["force_changed"] | ISSUE20_HEADERS_AND_LEDGER["force_changed"],
    "files": advance_graph(ISSUE541_AND_CUT17_LOCALFILE_LEDGER["files"], ISSUE20_HEADERS_AND_LEDGER["files"]),
}
ISSUE740_LIB_TRIG_HEADER = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset(),
    "files": {
        "include/Lib/trig.h": (None, "ed2c3832677714e887c1f67c160a578c3f471345bcfbdc30ef79f55db2d87c2e"),
    },
}
REMOVE_FAKE_D3DX_HEADERS = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "include/wwmath/d3dxmatrixinverse.h",
        "include/wwmath/d3dxvec3catmullrom.h",
        "src/code/libraries/source/wwvegas/wwmath/matrix3d.cpp",
        "src/code/libraries/source/wwvegas/wwmath/matrix4.cpp",
        "src/code/gameengine/source/gameclient/drawable.cpp",
    }),
    "files": {
        "include/WWMath/D3DXMatrixInverse.h": (
            "f33f2b9dda6ba88227b0530866648cfa1b052977b5303a7ab0b70cd8c137ef88",
            None,
        ),
        "include/WWMath/D3DXVec3CatmullRom.h": (
            "6d04f5c36ad46adee85e26cba9c8cf6c001192a0570aa432137d51073c0c09df",
            None,
        ),
    },
}
CONSOLIDATE_W3D_FILE_H = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "include/ww3d2/w3d_file.h",
        "include/ww3d2/w3d_emitter.h",
        "include/ww3d2/w3d_aggregate.h",
        "include/ww3d2/aggregatedefclass.h",
        "include/ww3d2/particlebufferclass.h",
        "include/ww3d2/particleemitterclass.h",
        "include/ww3d2/particleemitterdefclass.h",
        "include/ww3d2/seglinerendererclass.h",
        "include/ww3d2/streakrendererclass.h",
    }),
    "files": {
        "include/WW3D2/w3d_emitter.h": (
            "b17345d5f3bf4566948994eb93944bd4c78a7eef58cf55f4f7d29eb0260a6aa5",
            None,
        ),
        "include/WW3D2/w3d_aggregate.h": (
            "095830d7fa123904e1f76c828140a2bd9595a9bb9b88cd96e5a4e999a950a43f",
            None,
        ),
    },
}
REMOVE_FAKE_D3DX_AND_W3D_FILE_H = {
    "old_tus": REMOVE_FAKE_D3DX_HEADERS["old_tus"] | CONSOLIDATE_W3D_FILE_H["old_tus"],
    "new_tus": REMOVE_FAKE_D3DX_HEADERS["new_tus"] | CONSOLIDATE_W3D_FILE_H["new_tus"],
    "force_changed": REMOVE_FAKE_D3DX_HEADERS["force_changed"] | CONSOLIDATE_W3D_FILE_H["force_changed"],
    "files": advance_graph(REMOVE_FAKE_D3DX_HEADERS["files"], CONSOLIDATE_W3D_FILE_H["files"]),
}
LEDGER_BATCH_741_747_636_667_650 = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "src/code/gameengine/source/common/messagestream.cpp",
        "src/code/gameengine/source/common/recorder.cpp",
        "src/code/gameengine/source/common/rts/player.cpp",
        "src/code/gameengine/source/gameclient/gameclientdispatch.cpp",
        "src/code/gameengine/source/gameclient/gui/aptplayer.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptcampaignreview.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptconnectionscreen.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptdisconnectscreen.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptfiletransferpopup.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptmappreview.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptobjectivesmenu.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptonlinecustommatch.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptonlineshell.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptoptions.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptplayertribute.cpp",
        "src/code/gameengine/source/gameclient/livingworld/ingameui/strategicingameuicancelarmymembermovebutton.cpp",
        "src/code/gameengine/source/gameclient/livingworld/livingworldarmyicon.cpp",
        "src/code/gameengine/source/gameclient/messagestream/hintspy.cpp",
        "src/code/gameengine/source/gameclient/messagestream/hotkey.cpp",
        "src/code/gameengine/source/gameclient/system/image.cpp",
        "src/code/gameengine/source/gamelogic/object/update/aiupdate/giantbirdaiupdate.cpp",
        "src/code/gameengine/source/gamelogic/system/castlesystem.cpp",
        "src/code/gameengine/source/gamelogic/system/livingworld/livingworldarmy.cpp",
        "src/code/gameengine/source/gamelogic/system/livingworld/livingworldbattle.cpp",
        "src/code/gameengine/source/gamelogic/system/livingworld/livingworldbuildingtemplate.cpp",
        "src/code/gameengine/source/gamelogic/system/livingworld/livingworldlogic.cpp",
        "src/code/gameengine/source/gamelogic/system/livingworld/livingworldplayer.cpp",
        "src/code/gameengine/source/gamelogic/system/livingworld/livingworldregionmanager.cpp",
        "src/code/gameengine/source/gamelogic/system/livingworld/livingworldtutorialtask.cpp",
        "src/code/gameengine/source/gamenetwork/connection.cpp",
        "src/code/gameengine/source/gamenetwork/connectionmanager.cpp",
        "src/code/gameengine/source/gamenetwork/netcommandmsg.cpp",
        "src/code/gameengine/source/gamenetwork/netpacket.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/baseheightmap.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/aabtreebuilder.cpp",
        "src/unk/g003f3f83.cpp",
        "src/unk/g003f9edf.cpp",
        "src/unk/g00538ed1.cpp",
        "src/unk/o004df983.cpp",
        "src/unk/o0056b88a.cpp",
        "src/unk/o005e39f3.cpp",
        "src/unk/o005f594c.cpp",
        "src/unk/o005f5d25.cpp",
    }),
    "files": {
        "config/demangled.json": (
            "377220f3c36c898946fca178e2716885414f977ac2a99ea0c6599a73437dcb83",
            "df0160b78f92eaf9bf0cda53feab5f7b1d1f34bdb367a78d52a1269b7db9ea73",
        ),
        "config/map.json": (
            "c0ad4ebec2e66d966adeec25eda3bf2411629307b7f670e2f1c5e0d999e15aa6",
            "2fc722d19b8f4384b544724c394f23a34c9728bad04368ac9e19f714b00e1e76",
        ),
        "expected/Code/GameEngine/Source/GameLogic/System/LivingWorld/LivingWorldBattle.cpp.json": (
            "1efb91b9eedb9d4db9afea54820c85997f3cb53b105d56f55c527603e1f0a53d",
            "5c335d0d7e76b44b789cbc25fc4aa4a15405f4a443b8f397ee03664f075fb0c0",
        ),
        "expected/Code/GameEngine/Source/GameLogic/System/LivingWorld/LivingWorldBuildingTemplate.cpp.json": (
            "6eb4018740e2ea1b72dc5ea107f21c70b7870a67191039cccf44d992e4029ae0",
            "4d5edd0a986b928bf50ab0925555f0b3c036c63131833d9302abe39b0bf1936f",
        ),
        "expected/Code/GameEngine/Source/GameLogic/System/LivingWorld/LivingWorldLogic.cpp.json": (
            "2ae52bf7b6c1a7eacfd8f7aac93840e9b189c59f45abe9c0cc80676ec8b02e56",
            "5a699e50b78c9bc1a59fa5e6c29ccde40017bc502a8c78d25f3614884e100b67",
        ),
        "expected/Code/GameEngineDevice/Source/W3DDevice/GameClient/BaseHeightMap.cpp.json": (
            "ace5f5707c41798de084994847fe8bb9e7c687a380abf5369723f3a28de727dc",
            "b80d3085e54ad0e93e2bd0b2e9e1d0a1502830b4a960129f0ac3a69622a2a533",
        ),
        "expected/Code/Libraries/Source/WWVegas/WW3D2/aabtreebuilder.cpp.json": (
            "6887d403af5600d97029c3ff9c53a84fcf211dd09ff6314b9340a64505406a4b",
            "2c5ea5a52b18bde9ec7f2200641ab1cd697e2ead2a1a05c8fb18af573e73dc77",
        ),
        "expected/unk/O004DF983.cpp.json": (
            "f939aade5de589319813da4028c97f129aa15627375b3eb0deca6dd72207fb37",
            "fd80aa35e7d934eeb5c74ccc33873232e869cc398c9bd385ca8ab96e0ff8c146",
        ),
    },
}
REMOVE_GAMESPY_SDK_SHIMS = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "include/upstream/gamespy-sdk/shims/windows.h",
        "include/upstream/gamespy-sdk/shims/winsock.h",
        "src/code/libraries/source/gamespy/gamespy/chat/chatcallbacks.c",
        "src/code/libraries/source/gamespy/gamespy/chat/chatchannel.c",
        "src/code/libraries/source/gamespy/gamespy/chat/chathandlers.c",
        "src/code/libraries/source/gamespy/gamespy/chat/chatmain.c",
        "src/code/libraries/source/gamespy/gamespy/chat/chatsocket.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peerautomatch.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peercallbacks.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peerglobalcallbacks.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peerkeys.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peermain.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peermangle.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peeroperations.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peerping.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peerplayers.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peerqr.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peerrooms.c",
        "src/code/libraries/source/gamespy/gamespy/peer/peersb.c",
        "src/code/libraries/source/gamespy/gamespy/pinger/pingermain.c",
        "src/code/libraries/source/gamespy/gamespy/serverbrowsing/sb_serverlist.c",
        "src/gamespy-sdk/gamespy/available.c",
        "src/gamespy-sdk/gamespy/available.h",
        "src/gamespy-sdk/gamespy/darray.c",
        "src/gamespy-sdk/gamespy/darray.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttp.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttpascii.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttpbuffer.c",
        "src/gamespy-sdk/gamespy/ghttp/ghttpbuffer.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttpcallbacks.c",
        "src/gamespy-sdk/gamespy/ghttp/ghttpcallbacks.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttpcommon.c",
        "src/gamespy-sdk/gamespy/ghttp/ghttpcommon.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttpconnection.c",
        "src/gamespy-sdk/gamespy/ghttp/ghttpconnection.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttpmain.c",
        "src/gamespy-sdk/gamespy/ghttp/ghttpmain.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttppost.c",
        "src/gamespy-sdk/gamespy/ghttp/ghttppost.h",
        "src/gamespy-sdk/gamespy/ghttp/ghttpprocess.c",
        "src/gamespy-sdk/gamespy/ghttp/ghttpprocess.h",
        "src/gamespy-sdk/gamespy/gp/gp.c",
        "src/gamespy-sdk/gamespy/gp/gp.h",
        "src/gamespy-sdk/gamespy/gp/gpi.c",
        "src/gamespy-sdk/gamespy/gp/gpi.h",
        "src/gamespy-sdk/gamespy/gp/gpibuddy.c",
        "src/gamespy-sdk/gamespy/gp/gpibuddy.h",
        "src/gamespy-sdk/gamespy/gp/gpibuffer.c",
        "src/gamespy-sdk/gamespy/gp/gpibuffer.h",
        "src/gamespy-sdk/gamespy/gp/gpicallback.c",
        "src/gamespy-sdk/gamespy/gp/gpicallback.h",
        "src/gamespy-sdk/gamespy/gp/gpiconnect.c",
        "src/gamespy-sdk/gamespy/gp/gpiconnect.h",
        "src/gamespy-sdk/gamespy/gp/gpiinfo.c",
        "src/gamespy-sdk/gamespy/gp/gpiinfo.h",
        "src/gamespy-sdk/gamespy/gp/gpioperation.c",
        "src/gamespy-sdk/gamespy/gp/gpioperation.h",
        "src/gamespy-sdk/gamespy/gp/gpipeer.c",
        "src/gamespy-sdk/gamespy/gp/gpipeer.h",
        "src/gamespy-sdk/gamespy/gp/gpiprofile.c",
        "src/gamespy-sdk/gamespy/gp/gpiprofile.h",
        "src/gamespy-sdk/gamespy/gp/gpisearch.c",
        "src/gamespy-sdk/gamespy/gp/gpisearch.h",
        "src/gamespy-sdk/gamespy/gp/gpitransfer.c",
        "src/gamespy-sdk/gamespy/gp/gpitransfer.h",
        "src/gamespy-sdk/gamespy/gp/gpiunique.c",
        "src/gamespy-sdk/gamespy/gp/gpiunique.h",
        "src/gamespy-sdk/gamespy/gp/gpiutility.c",
        "src/gamespy-sdk/gamespy/gp/gpiutility.h",
        "src/gamespy-sdk/gamespy/gstats/gbucket.c",
        "src/gamespy-sdk/gamespy/gstats/gbucket.h",
        "src/gamespy-sdk/gamespy/gstats/gpersist.h",
        "src/gamespy-sdk/gamespy/gstats/gstats.c",
        "src/gamespy-sdk/gamespy/gstats/gstats.h",
        "src/gamespy-sdk/gamespy/hashtable.c",
        "src/gamespy-sdk/gamespy/hashtable.h",
        "src/gamespy-sdk/gamespy/md5.h",
        "src/gamespy-sdk/gamespy/md5c.c",
        "src/gamespy-sdk/gamespy/nonport.c",
        "src/gamespy-sdk/gamespy/nonport.h",
        "src/gamespy-sdk/gamespy/peer/peerhost.c",
        "src/gamespy-sdk/gamespy/qr2/qr2.c",
        "src/gamespy-sdk/gamespy/qr2/qr2.h",
        "src/gamespy-sdk/gamespy/qr2/qr2regkeys.c",
        "src/gamespy-sdk/gamespy/qr2/qr2regkeys.h",
        "src/gamespy-sdk/gamespy/serverbrowsing/sb_ascii.h",
        "src/gamespy-sdk/gamespy/serverbrowsing/sb_crypt.c",
        "src/gamespy-sdk/gamespy/serverbrowsing/sb_crypt.h",
        "src/gamespy-sdk/gamespy/serverbrowsing/sb_internal.h",
        "src/gamespy-sdk/gamespy/serverbrowsing/sb_queryengine.c",
        "src/gamespy-sdk/gamespy/serverbrowsing/sb_server.c",
        "src/gamespy-sdk/gamespy/serverbrowsing/sb_serverbrowsing.c",
        "src/gamespy-sdk/gamespy/serverbrowsing/sb_serverbrowsing.h",
        "src/gamespy-sdk/gamespy/stringutil.c",
        "src/gamespy-sdk/gamespy/stringutil.h",
    }),
    "files": {
        "include/upstream/gamespy-sdk/shims/windows.h": (
            "71b53b5a3b4bca5ccb5031bb73b68e03c8e183da7d2717969acc7f3279aa44bd",
            None,
        ),
        "include/upstream/gamespy-sdk/shims/winsock.h": (
            "6e9ce24d080d5203536b10b6f29b2372d30258ae323d3e52597975cce37adba4",
            None,
        ),
    },
}
MIGRATE_D3D8_TO_D3D9 = {
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "include/gameclient/w3dshadermanager.h",
        "include/ww3d2/dx8caps.h",
        "include/ww3d2/dxwrapper.h",
        "include/ww3d2/idirect3ddevice9.h",
        "include/ww3d2/indexbufferclass.h",
        "include/ww3d2/presentparameters9.h",
        "include/ww3d2/surfaceclass.h",
        "include/ww3d2/textureasset.h",
        "include/ww3d2/texturebaseclass.h",
        "include/ww3d2/vertexbufferclass.h",
        "include/ww3d2/vertexmaterialclass.h",
        "src/code/gameengine/source/gameclient/buffmanager.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/drawable/w3dscriptedmodeldraw.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/shadow/w3dbuffermanager.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/shadow/w3dprojectedshadow.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/terraintex.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3dbibbuffer.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3dbridgebuffer.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3dbuffbuffer.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3ddisplaystring.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3ddisplaystringmanager.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3droadbuffer.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3dshadermanager.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3dshroud.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/water/w3dwater.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/water/w3dwatertracks.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/boxrobj.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/camera.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/dx8caps.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/dx8fvf.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/dx8indexbuffer.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/dx8polygonrenderer.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/dx8renderer.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/dx8vertexbuffer.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/dxwrapper.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/fxshaderasset.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/fxshadergeometry.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/fxshaderrenderer.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/linegrp.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/mapper.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/matinfo.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/matpass.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/mesh.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/meshmatdesc.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/meshmdl.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/meshmdlio.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/part_buf.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/part_emt.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/pointgr.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/render2dsentence.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/renderasset.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/rinfo.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/scene.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/segline.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/sortingrenderer.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/static_sort_list.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/statistics.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/streak.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/streakrender.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/surfaceclass.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/texture.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/texturefilter.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/vertmaterial.cpp",
        "src/code/libraries/source/wwvegas/ww3d2/ww3d.cpp",
        "src/unk/g0010e482.cpp",
        "src/unk/g00171660.cpp",
        "src/unk/g001732e2.cpp",
        "src/unk/g00199fa5.cpp",
        "src/unk/o0006615f.cpp",
        "src/unk/o00075343.cpp",
        "src/unk/o00083b81.cpp",
        "src/unk/o00104d97.cpp",
        "src/unk/o0010737e.cpp",
        "src/unk/o001100e8.cpp",
        "src/unk/o0011c470.cpp",
        "src/unk/o0012d2c0.cpp",
        "src/unk/o0012d2d0.cpp",
        "src/unk/o0012e010.cpp",
        "src/unk/o00137550.cpp",
        "src/unk/o0013c930.cpp",
        "src/unk/o001433f0.cpp",
        "src/unk/o0016eaf0.cpp",
        "src/unk/o001820c0.cpp",
        "src/unk/o00191140.cpp",
        "src/unk/o001a09f0.cpp",
        "src/unk/o001a0c30.cpp",
        "src/unk/o0030d3ad.cpp",
    }),
    "files": {
        "config/map.json": (
            "2fc722d19b8f4384b544724c394f23a34c9728bad04368ac9e19f714b00e1e76",
            "9d0f75bde103b27a75b2842d6820f5f1012d3bbabf344993fb6e4c098a0c1c8a",
        ),
        "config/symbols.json": (
            "f6ca38d241c6a0ebd61180d927798d9a312d325123a94545f330b9a71dc414ee",
            "5a2e891adc6baf888c0da37923d08c5624bd31878dc219df9270c2cbe2bb96d2",
        ),
        "expected/Code/Libraries/Source/WWVegas/WW3D2/dx8caps.cpp.json": (
            "fdab1c0b2a20c801e6ec8e1b10aa0b023f04c49b041ca920b4ea332325267dcd",
            "3600185c56489a68cd80747810b8b9d31ecfaa41a9e6797790fa19e29cf8917e",
        ),
        "include/WW3D2/IDirect3DDevice9.h": (
            "913f4741eeaf37d1aff56062b00e81b5b3e7c4bbdc4d2c40c8a62dcb955ef6d0",
            None,
        ),
        "include/WW3D2/PresentParameters9.h": (
            "123c6e0084338c5a57012ae8df17d4c55a7f1aea309500e935fcc5e0ff770a0d",
            None,
        ),
    },
}
LEDGER_BATCH_632_633_746 = {
    "files": {
        "config/map.json": (
            "9d0f75bde103b27a75b2842d6820f5f1012d3bbabf344993fb6e4c098a0c1c8a",
            "b53e982e866f70a2ec32e791374718d6bef21109b0f1d191f2bdbd83b9cdd3d2",
        ),
        "expected/Code/GameEngine/Source/GameClient/Gui/ControlBar/ControlBar.cpp.json": (
            "25ae33896e106abdd255903f5fffade157e001a87fbf51336ee51cfb0ece9ae6",
            "fca9a2893a311fa889d429937aceb8071409af0a755b68bb642870e2894d93ef",
        ),
        "expected/Code/GameEngine/Source/GameLogic/Object/Behavior/BridgeScaffoldBehavior.cpp.json": (
            "f228dda937b76d844cfc3c7fcfabafc9c4bc8e12ce7b7daf968e5bdbcfbd8355",
            "9f3baf8367e13142fe0c42cc2b63cdde2bc080526743ef7e0f18671a6f221465",
        ),
        "expected/Code/GameEngineDevice/Source/W3DDevice/GameClient/Drawable/W3DScriptedModelDraw.cpp.json": (
            "cddc9960ee27959eacb636f2d29b82b8e0c1e119ad6a00cfe8a1f32ba6b8fd2a",
            "58fc0c51dcdd3fbc410b535f070e504ee38e7225522584e6a490de70e51a1bc1",
        ),
    },
    "old_tus": frozenset(),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "src/code/gameengine/source/common/audio/audioeventrts.cpp",
        "src/code/gameengine/source/common/system/upgrade.cpp",
        "src/code/gameengine/source/gameclient/buffmanager.cpp",
        "src/code/gameengine/source/gameclient/drawable.cpp",
        "src/code/gameengine/source/gameclient/gui/controlbar/controlbar.cpp",
        "src/code/gameengine/source/gameclient/gui/guicallbacks/apt/aptlanlobby.cpp",
        "src/code/gameengine/source/gameclient/livingworld/livingworld.cpp",
        "src/code/gameengine/source/gamelogic/object/behavior/bridgescaffoldbehavior.cpp",
        "src/code/gameengine/source/gamelogic/object/contain/siegeenginecontain.cpp",
        "src/code/gameengine/source/gamelogic/object/ghostobject.cpp",
        "src/code/gameengine/source/gamelogic/pathfinder/pathfinder_cell.cpp",
        "src/code/gameengine/source/gamelogic/skirmishai/aibuildable.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/drawable/w3dscriptedmodeldraw.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gameclient/w3dview.cpp",
        "src/code/gameenginedevice/source/w3ddevice/gamelogic/w3dghostobject.cpp",
        "src/unk/o0047bef8.cpp",
    }),
}

ISSUE634_PINGER_UNIT = {
    "files": {
        "config/map.json": (
            "b53e982e866f70a2ec32e791374718d6bef21109b0f1d191f2bdbd83b9cdd3d2",
            "7ef31f0552fe082ea8319fef38170f3e2f18da09b096d1344308c709bc1d404e",
        ),
        "expected/gamespy-sdk/GameSpy/peer/peerPing.c.json": (
            "36d17aef3e83252177ab2666aa65e854704563dc49e00216070d71787c0ee61e",
            None,
        ),
        "expected/gamespy-sdk/GameSpy/peer/peerPing.c.neighbours.json": (
            "38ed351327f70b8bae49009b20ec574d76f12bb766bef42092c9ce8802736ab2",
            None,
        ),
        "expected/unk/G006B2150.cpp.json": (
            "f53d37db01160761a6e28c5228bf76f418037322b8c708f6c06b806733db0d28",
            None,
        ),
        "expected/unk/G006B23A0.cpp.json": (
            "e37dc16b8942999720171791b3edbb37554a3e241f468d64468ab672b2dc19d6",
            None,
        ),
        "expected/unk/O006B22E0.cpp.json": (
            "940dcfce027678f80c98206ace972b0a5c2ec7f99eaee79582860282979cf352",
            None,
        ),
        "expected/unk/O006B22E0.cpp.neighbours.json": (
            "b7293d714f18951d9de20a5e6e1851db56b262c7426b1f38ec62742cd8ec6d24",
            None,
        ),
        "expected/unk/O006B2420.cpp.json": (
            "97eebb160b6ff8302c0ad06ae3ab8c7597c8ad14e297f915c6e22ab2f8aabd8f",
            None,
        ),
        "expected/unk/O006B2420.cpp.neighbours.json": (
            "922c4f4c4402c4192fb5ea2bd7f4744130cfd973ddbc3019f6cf3fe59af5bbae",
            None,
        ),
        "src/gamespy-sdk/GameSpy/peer/peerPing.c": (
            "6787c0a0137d04fafbe46bc8f28e6754a657111832ad3dc4c0f74685f4f96a7e",
            None,
        ),
        "src/unk/G006B2150.cpp": (
            "1cea00049884683ed53a4c932f17fdd4494c080fc264556c2932fae1e3d62f6b",
            None,
        ),
        "src/unk/G006B23A0.cpp": (
            "dde2c926c1058cd5ad2c34cb055d6f946ba1fbd276de5caa0b2d51c4956366c9",
            None,
        ),
        "src/unk/O006B22E0.cpp": (
            "2e77f5395c07a881cf561b39fa99622a06545190f6018716c391e76a985baac0",
            None,
        ),
        "src/unk/O006B2420.cpp": (
            "d202908d9387703f6cf9adadb03ba299d54c80c291f9a6d9d5a6a32fd833e56c",
            None,
        ),
    },
    "old_tus": frozenset({
        "src/gamespy-sdk/GameSpy/peer/peerPing.c",
        "src/unk/G006B2150.cpp",
        "src/unk/G006B23A0.cpp",
        "src/unk/O006B22E0.cpp",
        "src/unk/O006B2420.cpp",
    }),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "src/code/libraries/source/gamespy/gamespy/pinger/pingermain.c",
    }),
}


ISSUE742_SET_VIEWPORT_INLINE = {
    "files": {
        "config/map.json": (
            "7ef31f0552fe082ea8319fef38170f3e2f18da09b096d1344308c709bc1d404e",
            "3194f7848812b0cef7239e0a41ffd99b5d33811c395a04f4ce12ea06bb9a06fc",
        ),
        "expected/unk/O0007E6A6.cpp.json": (
            "c1ee24bdc8fa21f2a0a4bad406f3bb32d955ff00e5ff2e413764934527bc747f",
            None,
        ),
        "expected/unk/O0007E6A6.cpp.neighbours.json": (
            "c02a2bf3cdd270b9848e542403fade982655724678476c69f76e645cb674b5ef",
            None,
        ),
        "src/unk/O0007E6A6.cpp": (
            "224bbcdf31f5bd00ed4439ef696f1aa8fd8701725cdc18a721cae79c2ddae261",
            None,
        ),
    },
    "old_tus": frozenset({
        "src/unk/O0007E6A6.cpp",
    }),
    "new_tus": frozenset(),
    "force_changed": frozenset({
        "include/ww3d2/cameraclass.h",
    }),
}

TU_MOVE_MIGRATIONS = (
    CUT14_TOOLS_TU_MOVE,
    CUT16_WB_RENAMES,
    ISSUE541_PROFILES_AND_GAP_UNITS,
    CUT17_LOCALFILE_LEDGER,
    ISSUE541_AND_CUT17_LOCALFILE_LEDGER,
    ISSUE20_HEADERS_AND_LEDGER,
    CUT17_AND_ISSUE20_LEDGER,
    ISSUE541_CUT17_AND_ISSUE20_LEDGER,
    ISSUE740_LIB_TRIG_HEADER,
    REMOVE_FAKE_D3DX_AND_W3D_FILE_H,
    REMOVE_FAKE_D3DX_HEADERS,
    CONSOLIDATE_W3D_FILE_H,
    LEDGER_BATCH_741_747_636_667_650,
    REMOVE_GAMESPY_SDK_SHIMS,
    MIGRATE_D3D8_TO_D3D9,
    ISSUE742_SET_VIEWPORT_INLINE,
    ISSUE634_PINGER_UNIT,
    LEDGER_BATCH_632_633_746,
)


GATES_RUNTIME_WORKAROUNDS_GRAPH = {
    'tools/gates/README.md': ('cfe2a0d6068dd84a2eb81abc61d0822002d98305ec0dc01efc75a8b770e77007', '97a53c1d9e3437293ee52088beab184eea19f151c97f7b8023fcc420e7bea4f8'),
    'tools/gates/config.json': ('8efc59a0cd826377dc8b95515b6c7dfcbac9158e5bf7c03d3fa86a60f393c5c9', '4104c5f067425f29c152610e52f6efc99a382cb2c90a4e7b656d265244e17667'),
    'tools/gates/rules.py': ('6d404b8cea627498d7e36288f7c0f806b7debf1ec988eab1cec8d0f82e966459', '3914923d49f6beb8bd7bc778fce5ba6a61be63930d561260c106a06d4b1f4ec0'),
    'tools/gates/source_report.py': ('0b75222bfba1d4364cf90923e76b0ba7f01bcd95ca134c785b6771351975a148', '82583c7952eea59d39e4aca4dab7eaa7c27630a872c202cea5f041d9dc170f51'),
    'tools/gates/test_gate.py': ('d58da420612a4759917f742d7a8f9d97ff375e56594cf2d967ffce6c9806a8d0', '9260b4b952f5de58e8385a2ba733006ea9ab666db3d7f32a8b67b277d4f82a2a'),
}
GRAPH_CHANGES += (GATES_RUNTIME_WORKAROUNDS_GRAPH,) + tuple(
    advance_graph(previous, GATES_RUNTIME_WORKAROUNDS_GRAPH)
    for previous in GRAPH_CHANGES
    if (previous.keys() & GATES_RUNTIME_WORKAROUNDS_GRAPH.keys())
    and all(previous[path][1] == GATES_RUNTIME_WORKAROUNDS_GRAPH[path][0]
            for path in previous.keys() & GATES_RUNTIME_WORKAROUNDS_GRAPH.keys())
)
GRAPH_PATHS |= set(GATES_RUNTIME_WORKAROUNDS_GRAPH)

GATES_POLICY_DIGEST_TEST_GRAPH = {
    'tools/gates/test_public_delivery.py': ('88ba4d31b1a852140d889cb90859032e145eadf082cc56597e38456deb2f919a', 'd31813480912510404819a87bc9f2db13cd33c29c0fb5687ac9747f3963512b9'),
}
GRAPH_CHANGES += (GATES_POLICY_DIGEST_TEST_GRAPH,) + tuple(
    advance_graph(previous, GATES_POLICY_DIGEST_TEST_GRAPH)
    for previous in GRAPH_CHANGES
    if (previous.keys() & GATES_POLICY_DIGEST_TEST_GRAPH.keys())
    and all(previous[path][1] == GATES_POLICY_DIGEST_TEST_GRAPH[path][0]
            for path in previous.keys() & GATES_POLICY_DIGEST_TEST_GRAPH.keys())
)
GRAPH_PATHS |= set(GATES_POLICY_DIGEST_TEST_GRAPH)

GATES_LEDGER_RUNTIME_NAMES_GRAPH = {
    'tools/gates/README.md': ('97a53c1d9e3437293ee52088beab184eea19f151c97f7b8023fcc420e7bea4f8', '96b1d3d3bbad05f12f115b79f23e935c84b16dfcd15d43994877fdc7b832df50'),
    'tools/gates/ledger_runtime_names.py': (None, 'a6072aa277b6ab65eca533eb0b93d07a107f6a1cf84a831abdc75b04fefa3445'),
    'tools/gates/test_ledger_runtime_names.py': (None, 'ad9ad1dcfd805a4fb481ece2d2088cbb6a7c9de00c697d191a655e54b0094894'),
}
GRAPH_CHANGES += (GATES_LEDGER_RUNTIME_NAMES_GRAPH,) + tuple(
    advance_graph(previous, GATES_LEDGER_RUNTIME_NAMES_GRAPH)
    for previous in GRAPH_CHANGES
    if (previous.keys() & GATES_LEDGER_RUNTIME_NAMES_GRAPH.keys())
    and all(previous[path][1] == GATES_LEDGER_RUNTIME_NAMES_GRAPH[path][0]
            for path in previous.keys() & GATES_LEDGER_RUNTIME_NAMES_GRAPH.keys())
)
GRAPH_PATHS |= set(GATES_LEDGER_RUNTIME_NAMES_GRAPH)

GATES_VOID_ABI_CLASS_PTRS_GRAPH = {
    'tools/gates/void_abi.py': ('0e2871578628970fb952daef54d6f0a7221d8152c3630c06725e8d183d096f32', 'c6e0edf323440bcb063168e947bc3db73887805b0b8a096d607436c153984eb9'),
    'tools/gates/test_gate.py': ('9260b4b952f5de58e8385a2ba733006ea9ab666db3d7f32a8b67b277d4f82a2a', '7fef78228c245a95d7785bc10bb7e827a3da61c6c6b4dad7291b9c76107ed31a'),
    'tools/gates/test_public_delivery.py': ('d31813480912510404819a87bc9f2db13cd33c29c0fb5687ac9747f3963512b9', '76e40a3712f05db67580d5673a702c146e6bfe73c6a09f269fac2e4557d3d5a3'),
}
GRAPH_CHANGES += (GATES_VOID_ABI_CLASS_PTRS_GRAPH,) + tuple(
    advance_graph(previous, GATES_VOID_ABI_CLASS_PTRS_GRAPH)
    for previous in GRAPH_CHANGES
    if (previous.keys() & GATES_VOID_ABI_CLASS_PTRS_GRAPH.keys())
    and all(previous[path][1] == GATES_VOID_ABI_CLASS_PTRS_GRAPH[path][0]
            for path in previous.keys() & GATES_VOID_ABI_CLASS_PTRS_GRAPH.keys())
)
GRAPH_PATHS |= set(GATES_VOID_ABI_CLASS_PTRS_GRAPH)

COMPACT_SCOPE = {"ci/managed-state/state_delta.py", "ci/managed-state/state_origin.py",
                 "ci/managed-state/bundle.sha256"}
MAX_DIFF_BYTES = 4 * 1024**2
ANCESTOR_LIMIT = 128


def predecessors(root, main):
    return git(root, "rev-list", "--first-parent", "--skip=1", "--max-count",
               str(ANCESTOR_LIMIT), main).decode("ascii").splitlines()


def graph_blob(root, revision, path):
    entry = git(root, "ls-tree", revision, "--", path)
    if not entry:
        return None
    if not entry.startswith(b"100644 blob "):
        raise ValueError("A graph control no longer has its approved file mode.")
    return sha256(git(root, "show", f"{revision}:{path}")).hexdigest()


def graph_change(root, parent, main, paths=None):
    best = None
    try:
        for transition in GRAPH_CHANGES:
            if paths is not None and set(transition) != paths:
                continue
            if all(tuple(graph_blob(root, rev, path) for rev in (parent, main)) == expected
                   for path, expected in transition.items()):
                if best is None or len(transition) > len(best):
                    best = transition
    except (OSError, ValueError, CalledProcessError):
        return None
    return best


def tu_move_change(root, parent, main):
    try:
        for migration in TU_MOVE_MIGRATIONS:
            if (all(tuple(graph_blob(root, rev, path) for rev in (parent, main)) == expected
                    for path, expected in migration["files"].items())
                    and all(graph_blob(root, parent, tu) is None and graph_blob(root, main, tu) is not None
                            for tu in migration["new_tus"])):
                return migration
    except (OSError, ValueError, CalledProcessError):
        return None
    return None


def publication_path(root, parent, main, file):
    if file in GRAPH_PATHS:
        transition = graph_change(root, parent, main)
        return transition is not None and file in transition
    if file == STATE_FLOW:
        fingerprints = tuple(sha256(git(root, "show", f"{rev}:{file}")).hexdigest()
                             for rev in (parent, main))
        return fingerprints in STATE_FLOW_MIGRATIONS
    return (file in TRANSPORT or file.startswith(REPORT_DIRS)
            or ("/" not in file and file.endswith(".md")))


def _direct_transition(root, parent, main):
    """Return (changed, old_tus, new_tus, bundle_pair) for one direct diff, or None."""
    data = git(root, "diff-tree", "-r", "--raw", "-z", "--no-commit-id", "--no-renames",
               "--no-ext-diff", parent, main, "--")
    if len(data) > MAX_DIFF_BYTES:
        return None
    parts = data.split(b"\0")
    if parts.pop() != b"" or len(parts) % 2:
        return None
    move = tu_move_change(root, parent, main)
    move_files = move["files"] if move else {}
    move_new = move["new_tus"] if move else frozenset()
    changed = set(move.get("force_changed", ())) if move else set()
    for meta, name in zip(parts[::2], parts[1::2], strict=True):
        file = name.decode("utf-8", "strict")
        info = meta.split()
        regular = (len(info) == 5 and info[0] in {b":100644", b":000000"}
                   and info[1] in {b"100644", b"000000"} and info[4] in {b"A", b"D", b"M"})
        if regular and file in move_new:
            changed.add(file.casefold())
            continue
        if regular and file in move_files:
            continue
        if regular and publication_path(root, parent, main, file):
            continue
        # New/deleted paths, configuration, ledgers, and name or mode changes
        # may alter symbolic lookup or introduce an earlier include candidate.
        if (not file.startswith(("src/", "include/")) or len(info) != 5
                or info[:2] != [b":100644", b"100644"] or info[4] != b"M"):
            return None
        changed.add(file.casefold())
    old_only = move["old_tus"] if move else frozenset()
    new_only = move["new_tus"] if move else frozenset()
    transition = graph_change(root, parent, main)
    bundle_pair = transition.get(BUNDLE) if transition else None
    return changed, old_only, new_only, bundle_pair


def compose_transition(root, parent, main):
    """Return (changed, old_tus, new_tus, bundle_pair), composing consecutive first-parent steps if needed."""
    direct = _direct_transition(root, parent, main)
    if direct is not None:
        return direct
    try:
        chain = git(root, "rev-list", "--first-parent", "--reverse", "--max-count",
                    str(ANCESTOR_LIMIT + 1), f"{parent}..{main}").decode("ascii").splitlines()
    except (OSError, ValueError, CalledProcessError):
        return None
    if len(chain) < 2 or len(chain) > ANCESTOR_LIMIT:
        return None
    try:
        first_parent = git(root, "rev-parse", chain[0] + "^1").decode("ascii").strip()
    except (OSError, ValueError, CalledProcessError):
        return None
    if first_parent != parent:
        return None
    changed = set()
    old_tus = set()
    new_tus = set()
    bundle_pair = None
    prev = parent
    for cur in chain:
        step = _direct_transition(root, prev, cur)
        if step is None:
            return None
        step_changed, step_old, step_new, step_bundle = step
        changed |= step_changed
        for tu in step_old:
            if tu in new_tus:
                new_tus.remove(tu)
            else:
                old_tus.add(tu)
        for tu in step_new:
            if tu in old_tus:
                old_tus.remove(tu)
                changed.add(tu.casefold())
            else:
                new_tus.add(tu)
        if step_bundle is not None:
            if bundle_pair is None:
                bundle_pair = step_bundle
            elif bundle_pair[1] == step_bundle[0]:
                bundle_pair = (bundle_pair[0], step_bundle[1])
            else:
                return None
        prev = cur
    return changed, frozenset(old_tus), frozenset(new_tus), bundle_pair


def source_changes(root, parent, main):
    """Return the dirty source paths, or None if the transition needs a full rebuild."""
    res = compose_transition(root, parent, main)
    return res[0] if res is not None else None


def unaffected(evidence, changed):
    clean = {}
    for tu, entry in evidence.items():
        row = entry["compiler"]
        if tu.casefold() in changed or (changed and row["status"] != "compiled"):
            continue
        # Only installed and authenticated receipts can supply these dependencies.
        if any(path.casefold() in changed for path, _ in row.get("dependencies", ())):
            continue
        clean[tu] = entry
    return clean


def inherited(root, main, snapshot, *, warn=print):
    if not snapshot or not snapshot.is_file():
        return {}
    try:
        old = load(snapshot, LIMIT)
        parent = old.get("sha", "")
        if not isinstance(parent, str) or not OID.fullmatch(parent) or parent not in predecessors(root, main):
            return {}
        composed = compose_transition(root, parent, main)
        if composed is None:
            return {}
        changed, old_only, new_only, bundle_pair = composed
        tree = git(root, "rev-parse", parent + "^{tree}").decode().strip()
        units = authoritative_units(root, main)
        old_set, unit_set = set(old["evidence"]), set(units)
        if (not (old_only <= old_set) or not (new_only <= unit_set)
                or (old_only & unit_set) or (new_only & old_set)
                or (old_set - old_only) != (unit_set - new_only)):
            return {}
        kept = [name for name in units if name not in new_only]
        keys = ("source", "contract")
        requests = [{"tu": name, **{key: old["evidence"][name]["compiler"][key] for key in keys}}
                    for name in kept]
        if not all(safe_tu(item["tu"]) for item in requests):
            return {}
        identity = header(parent, tree)
        if bundle_pair and identity["policy"] == bundle_pair[1]:
            identity["policy"] = bundle_pair[0]
        return unaffected(cached_pairs(old, identity, requests), changed)
    except (OSError, ValueError, TypeError, KeyError, AttributeError, CalledProcessError):
        warn("Previous exact state cannot be reused. A full initial refresh is required.")
        return {}


def merge(required, measured, carried):
    if set(measured) & set(carried) or set(required) != set(measured) | set(carried):
        raise ValueError("The new protected state does not cover each translation unit exactly once.")
    return measured | carried

