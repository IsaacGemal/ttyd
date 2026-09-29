use std::fmt;

use anyhow::Context as _;
use hashbrown::HashMap;
use object::{File, Object as _, ObjectSection as _, ObjectSymbol as _};

use crate::opcode::Opcode;

#[derive(Debug, PartialEq, Eq)]
pub(crate) enum ExpressionType {
    Address,
    Pointer,
    Float,
    UserFlag,
    UserWork,
    GlobalSavedWork,
    LocalSavedWork,
    GlobalSavedWorkFlag,
    LocalSavedWorkFlag,
    GlobalFlag,
    LocalFlag,
    GlobalWork,
    LocalWork,
    Immediate,
}

impl ExpressionType {
    /// Maps an index to the "region" it belongs to.
    pub fn category(index: i32) -> Self {
        match index {
            i32::MIN..=-270000000 => Self::Address,
            -269999999..=-250000000 => Self::Pointer,
            -249999999..=-220000000 => Self::Float,
            -219999999..=-200000000 => Self::UserFlag,
            -199999999..=-180000000 => Self::UserWork,
            -179999999..=-160000000 => Self::GlobalSavedWork,
            -159999999..=-140000000 => Self::LocalSavedWork,
            -139999999..=-120000000 => Self::GlobalSavedWorkFlag,
            -119999999..=-100000000 => Self::LocalSavedWorkFlag,
            -99999999..=-80000000 => Self::GlobalFlag,
            -79999999..=-60000000 => Self::LocalFlag,
            -59999999..=-40000000 => Self::GlobalWork,
            -39999999..=-20000000 => Self::LocalWork,
            _ => Self::Immediate,
        }
    }

    /// Decodes an index and returns the "relative" index inside that category.
    pub fn decode(index: i32) -> i32 {
        match Self::category(index) {
            Self::Address | Self::Pointer | Self::Immediate => index,
            Self::Float => index + 230000000,
            Self::UserFlag => index + 210000000,
            Self::UserWork => index + 190000000,
            Self::GlobalSavedWork => index + 170000000,
            Self::LocalSavedWork => index + 150000000,
            Self::GlobalSavedWorkFlag => index + 130000000,
            Self::LocalSavedWorkFlag => index + 110000000,
            Self::GlobalFlag => index + 90000000,
            Self::LocalFlag => index + 70000000,
            Self::GlobalWork => index + 50000000,
            Self::LocalWork => index + 30000000,
        }
    }
}

#[derive(Debug, Clone)]
pub(crate) enum Arg {
    /// Encountered a symbol at this offset.
    Symbol { name: String },
    /// The expression is a stored float (see check_float in manager/evtmgr_cmd).
    Float(f32),
    /// The expression represents a value stored inside an intermediate location.
    Stored { location: &'static str, value: i32 },
    /// The expression is an immediate, no decoding necessary.
    Imm(i32),
}

impl fmt::Display for Arg {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Symbol { name } => write!(f, "{name}"),
            Self::Float(value) => write!(f, "FLOAT({}f)", short_float(*value)),
            Self::Stored { location, value } => write!(f, "{location}({value})"),
            Self::Imm(value) => write!(f, "{value}"),
        }
    }
}

/// The shortest decimal that FLOAT() encodes to the same word: FLOAT truncates value * 1024 to an integer.
fn short_float(value: f32) -> String {
    let word = (value * 1024.0) as i32;
    for digits in 1..=8 {
        let text = format!("{value:.digits$}");
        if let Ok(parsed) = text.parse::<f32>() {
            if (parsed * 1024.0) as i32 == word {
                let text = text.trim_end_matches('0');
                return if text.ends_with('.') { format!("{text}0") } else { text.to_string() };
            }
        }
    }
    format!("{value:?}")
}

/// A C string literal's contents: everything passes through except quotes, backslashes and control characters.
fn c_escape(text: &str) -> String {
    let mut out = String::new();
    for c in text.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            '\n' => out.push_str("\\n"),
            '\t' => out.push_str("\\t"),
            c if (c as u32) < 0x20 || c as u32 == 0x7F => out.push_str(&format!("\\{:03o}", c as u32)),
            c => out.push(c),
        }
    }
    out
}

pub(crate) struct EventScript<'a> {
    /// The parsed `File` handle for obtaining sections and symbols.
    file: File<'a>,
    /// The raw data backing the actual script.
    data: &'a [u8],
    /// Mapping from section-relative offset -> (symbol_name, addend)
    lookup: &'a HashMap<u64, (String, object::SymbolIndex)>,
    /// Section-relative address of [`Self::data`]
    base: u64,
    /// Current position within [`Self::data`]
    pos: usize,
}

impl<'a> EventScript<'a> {
    pub fn new(file: File<'a>, data: &'a [u8], base: u64, lookup: &'a HashMap<u64, (String, object::SymbolIndex)>) -> Self {
        Self { file, data, lookup, base, pos: 0 }
    }

    pub fn remaining(&self) -> usize {
        self.data.len() - self.pos
    }

    pub fn offset(&self) -> u64 {
        self.base + self.pos as u64
    }

    pub fn read_i32(&mut self) -> anyhow::Result<i32> {
        let end = self.pos + 4;
        let word = *self.data[self.pos..].first_chunk::<4>().context("Reached end of the stream")?;
        self.pos = end;
        Ok(i32::from_be_bytes(word))
    }

    pub fn read_arg(&mut self) -> anyhow::Result<Arg> {
        let offset = self.offset();
        let raw = self.read_i32()?;
        let category = ExpressionType::category(raw);

        // There may be a symbol at this offset but doesn't show up because object didn't resolve it.
        if let Some((name, index)) = self.lookup.get(&offset) {
            let symbol = self.file.symbol_by_index(*index)?;

            let Some(section_index) = symbol.section_index() else {
                return Ok(Arg::Symbol { name: name.to_string() });
            };

            let section = self.file.section_by_index(section_index)?;
            if ![".rodata", ".sdata2"].contains(&section.name()?) {
                return Ok(Arg::Symbol { name: name.to_string() });
            }

            let bytes = section
                .data_range(symbol.address(), symbol.size())?
                .context("symbol range out of section bounds")?;
            let text = &bytes[..bytes.iter().position(|&b| b == 0).unwrap_or(bytes.len())];
            let (cow, _, had_errors) = encoding_rs::SHIFT_JIS.decode(text);
            match had_errors {
                true => Ok(Arg::Symbol { name: name.to_string() }),
                false => Ok(Arg::Symbol { name: format!("STRING(\"{}\")", c_escape(&cow)) }),
            }
        } else {
            match category {
                // No relocation here, so not a pointer: -250000000 is the "no value" sentinel scripts pass to
                // battle commands, anything else a plain constant.
                ExpressionType::Address | ExpressionType::Pointer => Ok(match raw {
                    -250000000 => Arg::Symbol { name: "EVTDAT_POINTER_MAX".to_string() },
                    _ => Arg::Symbol { name: format!("0x{:08X}", raw as u32) },
                }),
                ExpressionType::Float => {
                    let value = ExpressionType::decode(raw) as f32;
                    Ok(Arg::Float(value / 1024.0))
                }
                ExpressionType::UserFlag => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "UF", value })
                }
                ExpressionType::UserWork => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "UW", value })
                }
                ExpressionType::GlobalSavedWork => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "GSW", value })
                }
                ExpressionType::LocalSavedWork => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "LSW", value })
                }
                ExpressionType::GlobalSavedWorkFlag => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "GSWF", value })
                }
                ExpressionType::LocalSavedWorkFlag => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "LSWF", value })
                }
                ExpressionType::GlobalFlag => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "GF", value })
                }
                ExpressionType::LocalFlag => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "LF", value })
                }
                ExpressionType::GlobalWork => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "GW", value })
                }
                ExpressionType::LocalWork => {
                    let value = ExpressionType::decode(raw);
                    Ok(Arg::Stored { location: "LW", value })
                }
                ExpressionType::Immediate => Ok(Arg::Imm(raw)),
            }
        }
    }

    pub fn extract(&mut self, symbol: &str) -> anyhow::Result<()> {
        let mut indentation = 1;
        println!("EVT_BEGIN({})", symbol);
        while self.remaining() >= 4 {
            let command = self.read_i32()?;
            let argc = (command >> 16) as usize;
            let opcode_num = command & 0xFFFF;
            let opcode = Opcode::from_repr(opcode_num as usize)
                .with_context(|| format!("unknown opcode {opcode_num:#04X}"))?;
            let opcode_str: &'static str = opcode.decomp_macros();
            opcode.decrease_indent(&mut indentation);
            match opcode.is_tag() {
                true => println!("{}{opcode_str}", "    ".repeat(indentation)),
                false => {
                    let args = (0..argc).map(|_| self.read_arg()).collect::<anyhow::Result<Vec<_>>>()?;
                    let rendered = args
                        .into_iter()
                        .map(|arg0: Arg| Arg::to_string(&arg0))
                        .collect::<Vec<_>>()
                        .join(", ");
                    println!("{}{opcode_str}({rendered})", "    ".repeat(indentation));
                }
            }
            opcode.increase_indent(&mut indentation);
            // Past the last END_SCRIPT, the symbol may hold alignment padding
            if opcode == Opcode::EndScript && self.data[self.pos..].iter().all(|&b| b == 0) {
                break;
            }
        }
        println!("EVT_END()");
        Ok(())
    }
}
