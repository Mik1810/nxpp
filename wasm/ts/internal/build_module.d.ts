declare module "*.mjs" {
  const createNxppModule: (options?: Record<string, unknown>) => Promise<unknown>;
  export default createNxppModule;
}
