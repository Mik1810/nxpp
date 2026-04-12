import createNxppModule from "./build/nxpp_node.mjs";

export async function createNxpp(options = {}) {
  return createNxppModule(options);
}

const nxpp = await createNxppModule();

export default nxpp;