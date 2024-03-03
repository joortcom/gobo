#!/usr/bin/env python3

import pandas as pd


# map each field to its *direct* superclasses' fields that it inherited redefined or renamed
# field => set(field)
parent_fields = {}

# map each field to its subclasses' fields at *any* level
# field => {class name: {field: set(path)}}
descendant_fields = {}


# walk the parent_fields dict all the way up to the root from arg: path[ ... field]
def mark_descendants(path):
  head_field = path[0]  # head of the path!
  if head_field not in parent_fields:  # head_field has no parent_fields
    return
  parents = parent_fields[head_field]

  class_name, field_name = path[-1].split(".")  # only take the leaf field!

  for parent_field in parents:
    curr_path = (parent_field,) + path
    if parent_field not in descendant_fields:
      descendant_fields[parent_field] = {}
    if class_name not in descendant_fields[parent_field]:
      descendant_fields[parent_field][class_name] = {}
    if field_name not in descendant_fields[parent_field][class_name]:
      descendant_fields[parent_field][class_name][field_name] = set()
    descendant_fields[parent_field][class_name][field_name].add(curr_path)

    mark_descendants(curr_path)  # recursive call


def detect_diamond(fn):
  edges = pd.read_csv(fn, sep="[,\.]", skiprows=3, skipfooter=1, engine='python',
      names=["kind", "src_class", "src_field", "tgt_class", "tgt_field"])
  # print(edges)

  # populate the direct parent_fields dict
  for i, edge in edges.iterrows():
    # print(edge)
    tgt = edge["tgt_class"] + "." + edge["tgt_field"]
    src = edge["src_class"] + "." + edge["src_field"]
    if tgt not in parent_fields:
      parent_fields[tgt] = set()
    parent_fields[tgt].add(src)
  print("total fields: ", len(parent_fields))

  for field in parent_fields.keys():  # mark all the field's ancestors at *any* level
    mark_descendants((field,))

  # check descendant_fields, if any field mapped into a same class' multiple target field
  for field, clazzs in descendant_fields.items():
    for class_name, field_paths in clazzs.items():
      if len(field_paths.keys()) >= 2:
        print("diamond: ", field, " => ", class_name)
        for new_field, path in field_paths.items():
          print("  ", new_field, path)



if __name__ == "__main__":
  fn = "inherited_fields.path"
  detect_diamond(fn)
