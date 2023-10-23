import 'dart:io';

void main(List<String> args) {
  if (args.length != 2) {
    print('Usage: dart replace-hash.dart <hash> <filename>');
    exit(1);
  }

  String hash = args[0];
  String filename = args[1];

  File file = new File(filename);
  List<String> lines = file.readAsLinesSync();

  for (int i = 0; i < lines.length; i++) {
    if(lines[i].contains("'dart_revision':")){
      List<String> fields = lines[i].split(":");
      String replacedHash = fields[0] + ": '" + hash + "',";
      lines[i] = replacedHash;
      break;
    }
  }

  file.writeAsStringSync(lines.join('\n')+"\n");
}
