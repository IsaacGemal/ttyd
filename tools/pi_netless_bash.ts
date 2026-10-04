/**
 * pi extension: the agent's `bash` tool, run without network.
 *
 *   pi -e tools/pi_netless_bash.ts ...     (as wave_launch.py runs it, from the read-only base)
 *
 * `-e` alone replaces pi's built-in bash; `--exclude-tools bash` would also remove this extension's tool (pi applies it
 * to extension tools), leaving the agent with no bash at all.
 *
 * Same interface and output as pi's built-in bash (it is pi's own createBashTool, so timeouts, output caps and
 * truncation are unchanged); each command runs as
 *   bwrap --unshare-net --die-with-parent --dev-bind / / --chdir <cwd> -- /bin/bash -c <command>
 * a nested namespace inside the agent's existing sandbox with no network but loopback. pi itself keeps its API
 * connection, since only the commands the model runs go through here. The commands also don't see the API key: the
 * variables named in PI_NETLESS_DROP (set by wave_launch.py to its KEY_ENV) and anything named like a key, token or
 * secret are removed from their environment, so a command can't write the key into a file that gets collected.
 */
import type { ExtensionAPI } from "@earendil-works/pi-coding-agent";
import { createBashTool } from "@earendil-works/pi-coding-agent";

const q = (s: string) => `'${s.replace(/'/g, `'\\''`)}'`;
const drop = new Set((process.env.PI_NETLESS_DROP ?? "").split(",").filter(Boolean));
const secret = /(API_?KEY|TOKEN|SECRET|PASSWORD)/i;

export default function (pi: ExtensionAPI) {
	const bashTool = createBashTool(process.cwd(), {
		spawnHook: ({ command, cwd, env }) => ({
			command: `exec bwrap --unshare-net --die-with-parent --dev-bind / / --chdir ${q(cwd)} -- /bin/bash -c ${q(command)}`,
			cwd,
			env: Object.fromEntries(
				Object.entries(env).filter(([k]) => !drop.has(k) && !secret.test(k) && k !== "PI_NETLESS_DROP"),
			),
		}),
	});
	pi.registerTool({
		...bashTool,
		name: "bash",
		execute: async (id, params, signal, onUpdate, _ctx) => bashTool.execute(id, params, signal, onUpdate),
	});
}
