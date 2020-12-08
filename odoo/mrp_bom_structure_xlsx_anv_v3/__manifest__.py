# Copyright 2018 Eficent Business and IT Consulting Services S.L.
#   (http://www.eficent.com)
# License AGPL-3.0 or later (https://www.gnu.org/licenses/agpl.html).

{
    'name': "MRP BOM Structure XLSX ANV V3",
    'version': '12.0.2.0.0',
    'category': 'Manufacturing',
    'summary': 'Export BoM Structure and Cost to Excel .XLSX',
    'author': "James Morritt, Aeronavics",
    'website': 'https://github.com/Aeronavics/modular_feather',
    'license': 'AGPL-3',
    "depends": ['report_xlsx', 'mrp'],
    "data": [
        'report/bom_structure_xlsx.xml',
    ],
    "installable": True
}
