import qbs
import qbs.FileInfo

Module {
	Depends { name: "Qt.restclient" }

	property string qrestbuilderName: "qrestbuilder"

	version: Qt.restclient.version

	FileTagger {
		fileTags: ["restclient-xml"]
		patterns: ["*.rc.xml"]
	}

	Rule {
		inputs: ["restclient-xml"]

		Artifact {
			filePath: input.baseName + ".h"
			fileTags: ["hpp"]
		}
		Artifact {
			filePath: input.baseName + ".cpp"
			fileTags: ["cpp"]
		}

		prepare: {
			var cmd = new Command();
			cmd.description = "generating rest-api class" + input.fileName;
			cmd.highlight = "codegen";
			cmd.program = FileInfo.joinPaths(product.moduleProperty("Qt.core", "binPath"),
											 product.moduleProperty("Qt.restbuilder", "qrestbuilderName"));
			cmd.arguments = [
				"--in", input.filePath,
				"--header", outputs["hpp"][0].filePath,
				"--impl", outputs["cpp"][0].filePath
			];
			return cmd;
		}
	}
}
