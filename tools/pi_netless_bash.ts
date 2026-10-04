/**
 * pi extension: the agent's `bash` tool, run without network.
 *
 *   pi --exclude-tools bash -e tools/pi_netless_bash.ts ...     (as wave_launch.py runs it)
 *
 * Same interface and output as pi's built-in bash (it is pi's own createBashTool, so timeouts, output caps and
 * truncation are unchanged); each command runs as
 *   bwrap --unshare-net --die-with-parent --dev-bind / / --chdir <cwd> -- /bin/bash -c <command>
 * a nested namespace inside the agent's existing sandbox with no network but loopback. pi itself keeps its API
 * connection, since only the commands the model runs go through here. Agents fetched code with curl and git clone
 * through bash (wave 74 rsh_00, win_04); every fetch went through this tool, and pi has no other networked tool.
 */
import type { ExtensionAPI } from "@earendil-works/pi-coding-agent";
import { createBashTool } from "@earendil-works/pi-coding-agent";

const q = (s: string) => `'${s.replace(/'/g, `'\\''`)}'`;

export default function (pi: ExtensionAPI) {
	const bashTool = createBashTool(process.cwd(), {
		spawnHook: ({ command, cwd, env }) => ({
			command: `exec bwrap --unshare-net --die-with-parent --dev-bind / / --chdir ${q(cwd)} -- /bin/bash -c ${q(command)}`,
			cwd,
			env,
		}),
	});
	pi.registerTool({
		...bashTool,
		name: "bash",
		execute: async (id, params, signal, onUpdate, _ctx) => bashTool.execute(id, params, signal, onUpdate),
	});
}
