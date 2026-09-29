use std::path::PathBuf;

use anyhow::Context;
use hashbrown::HashMap;
use object::{Object, ObjectSection, ObjectSymbol, RelocationTarget};

mod cli;
mod opcode;
mod reader;

use crate::reader::EventScript;

const MODULES: [&str; 31] = [
    "aaa", "aji", "bom", "dig", "dmo", "dou", "eki", "end", "gon", "gor", "gra", "hei", "hom", "jin", "jon",
    "kpa", "las", "moo", "mri", "muj", "nok", "pik", "qiz", "rsh", "sys", "tik", "tou", "tou2", "usu", "win",
    "yuu",
];

struct EventReader {
    lookup: HashMap<u64, (String, object::SymbolIndex)>,
    bytes: Vec<u8>,
}

impl EventReader {
    fn initialize(module: &str, symbol: &str, address: Option<u64>, relative_path: PathBuf) -> anyhow::Result<Self> {
        const BUILD_PATH: &str = "build/G8MJ01/";

        // First, let's resolve the current module and symbol, so we can grab all relocations from that
        // section.
        let path = match module {
            module if MODULES.contains(&module) => {
                relative_path.join(BUILD_PATH).join(module).join(format!("{module}.plf"))
            }
            "MarioSt" | "dol" => relative_path.join(BUILD_PATH).join("MarioSt.elf"),
            _ => return Err(anyhow::anyhow!("Unknown module name: {module}")),
        };

        let bytes = std::fs::read(&path).context(format!("Unable to open file: {path:?}"))?;
        let file = object::File::parse(&*bytes)?;

        let symbol = find_symbol(&file, symbol, address).context("symbol not found")?;
        let section_index = symbol.section_index().context("symbol has no section")?;
        let section = file.section_by_index(section_index)?;

        // Now that we have the section, let's process all relocations so we can resolve them.
        let mut lookup = HashMap::new();
        for (offset, reloc) in section.relocations() {
            if let RelocationTarget::Symbol(index) = reloc.target() {
                let name = file.symbol_by_index(index)?.name().unwrap_or("?").to_string();
                lookup.insert(offset, (name, index));
            }
        }

        Ok(Self { lookup, bytes })
    }

    fn get_script<'a>(&'a self, symbol: &str, address: Option<u64>) -> Option<EventScript<'a>> {
        let file = object::File::parse(&*self.bytes).ok()?;

        let symbol = find_symbol(&file, symbol, address)?;
        let section_index = symbol.section_index()?;
        let section = file.section_by_index(section_index).ok()?;

        let script = section.data_range(symbol.address(), symbol.size()).ok()??;
        let base = symbol.address();
        let event = EventScript::new(file, script, base, &self.lookup);
        Some(event)
    }
}

/// The symbol with this name, or with this name at this section address when several share the name.
fn find_symbol<'data, 'file>(
    file: &'file object::File<'data>,
    name: &str,
    address: Option<u64>,
) -> Option<object::Symbol<'data, 'file>> {
    match address {
        Some(address) => file.symbols().find(|s| s.name() == Ok(name) && s.address() == address),
        None => file.symbol_by_name(name),
    }
}

fn main() -> anyhow::Result<()> {
    let cli: cli::TopLevel = argp::parse_args_or_exit(argp::DEFAULT);

    match cli.nested {
        cli::NestedCommands::Extract(cmd) => {
            let address = cmd
                .address
                .as_deref()
                .map(|a| u64::from_str_radix(a.trim_start_matches("0x"), 16))
                .transpose()
                .context("address must be hex")?;
            let reader = EventReader::initialize(&cmd.module, &cmd.symbol, address, cmd.relative_path.into())?;
            let mut script = reader
                .get_script(&cmd.symbol, address)
                .context(format!("Symbol wasn't found in module: {} -> {}", cmd.module, cmd.symbol))?;
            script.extract(&cmd.symbol)?;
        }
    }

    Ok(())
}
