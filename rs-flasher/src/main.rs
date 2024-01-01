use chrono::DateTime;
use espflash;
use espflash::cli::config::Config;
use espflash::cli::{connect, ConnectArgs, EspflashProgress};
use espflash::logging::initialize_logger;
use log::LevelFilter;
use miette::Result;
use serde::Deserialize;
use std::io;

fn main() -> Result<()> {
    miette::set_panic_hook();
    initialize_logger(LevelFilter::Info);

    let build_info_str = include_str!("../resources/buildinfo.json");

    let build_info: BuildInfo = serde_json::from_str(build_info_str).unwrap();

    // Display the disclaimer
    let disclaimer = format!(
        r#"
KNOMI Firmware Installer by DiverOfDark
==================================================================

Firmware info: https://github.com/DiverOfDark/KNOMI
Branch: {} ({})
Source Timestamp: {}
Binary Timestamp: {}

==================================================================

This tool will flash alternative KNOMI firmware to your device.
Please put your KNOMI into flash mode (press button and connect cable).

Once you do that - you can click on continue.

Do you want to continue? (yes/no): "#,
        build_info.branch,
        build_info.commit,
        format_time(build_info.commit_time),
        format_time(build_info.build_time)
    );

    println!("{}", disclaimer);

    // Get user input
    let mut user_input = String::new();
    io::stdin().read_line(&mut user_input).unwrap();

    user_input = user_input.trim().to_lowercase();

    // Check if the user wants to continue
    if user_input == "yes" {
        println!("Starting firmware flashing tool...");
        let config = Config::default();
        let addr = 0x1000;

        let conn = ConnectArgs {
            baud: Some(460800),
            no_stub: false,
            port: None,
        };
        let mut flasher = connect(&conn, &config)?;
        let bytes = include_bytes!("../resources/firmware_full.bin");

        flasher.write_bin_to_flash(
            addr,
            &bytes.as_slice(),
            Some(&mut EspflashProgress::default()),
        )?;

        println!("Firmware flashing completed. Please unplug your KNOMI and plug it back.");
        println!("Press Enter to exit");
        io::stdin().read_line(&mut user_input).unwrap();
    } else {
        println!("Firmware flashing canceled.");
        std::process::exit(1); // Exit the program with an error code
    }

    // full flash = espflash.exe write-bin -b 460800 0x1000 firmware_full.bin
    Ok(())
}

fn format_time(time: String) -> String {
    let parse1 = DateTime::parse_from_str(&time, "%Y-%m-%dT%H:%M:%S.%f%z").unwrap();
    parse1.format("%Y-%m-%d %H:%M:%S").to_string()
}

#[derive(Deserialize)]
#[serde(rename_all = "camelCase")]
struct BuildInfo {
    commit: String,
    branch: String,
    commit_time: String,
    build_time: String,
}
