# Copyright 2017 Eficent Business and IT Consulting Services S.L.
#   (http://www.eficent.com)
# License AGPL-3.0 or later (https://www.gnu.org/licenses/agpl.html).

import logging
from odoo import models
from odoo.tools.translate import _

_logger = logging.getLogger(__name__)


class BomStructureXlsx(models.AbstractModel):
    _name = 'report.mrp_bom_structure_xlsx.bom_structure_xlsx'
    _inherit = 'report.report_xlsx.abstract'

    def print_bom_children(self, workbook, ch, sheet, row, level, cost, total_qty, qty, bom):
        bold = workbook.add_format({'bold': True})
        i, j = row, level
        j += 1
        total_qty = total_qty*qty
        sheet.write(i, 1, bom)
        if ch.child_line_ids:
            if str(ch.product_id.default_code) == 'False':
                sheet.write(i, 2, '  ' * j + 'CODE_NOT_FOUND', bold)
            else:
                sheet.write(i, 2, '  ' * j + str(ch.product_id.default_code), bold)
            sheet.write(i, 3, ch.product_id.display_name or '', bold)
            sheet.write(i, 5, ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id) or '', bold)
            sheet.write(i, 6, ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id)*total_qty or '', bold)            
        else:
            if str(ch.product_id.default_code) == 'False':
                sheet.write(i, 2, '  ' * j + 'CODE_NOT_FOUND')
            else:
                sheet.write(i, 2, '  ' * j + str(ch.product_id.default_code))
            sheet.write(i, 3, ch.product_id.display_name or '')
            if ch.product_id.seller_ids:
                sheet.write(i, 4, ch.product_id.seller_ids[0].display_name)
                sheet.write(i, 7, ch.product_id.seller_ids[0].price or '')
                sheet.write(i, 8, ch.product_id.seller_ids[0].price * ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id)*total_qty or '')
                if ch.product_id.seller_ids[0].price and ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id):
                    cost += ch.product_id.seller_ids[0].price * ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id)*total_qty                
            else:
                sheet.write(i, 7, ch.product_id.standard_price or '')
                sheet.write(i, 8, ch.product_id.standard_price * ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id)*total_qty or '')
                if ch.product_id.standard_price and ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id):
                    cost += ch.product_id.standard_price * ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id)*total_qty                
            sheet.write(i, 5, ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id) or '')
            sheet.write(i, 6, ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id)*total_qty or '')
            
        i += 1
        for child in ch.child_line_ids:
            i, cost = self.print_bom_children(workbook, child, sheet, i, j, cost, total_qty, ch.product_uom_id._compute_quantity(ch.product_qty, ch.product_id.uom_id), ch.product_id.display_name)
        j -= 1
        total_qty = total_qty/qty
        return i, cost

    def generate_xlsx_report(self, workbook, data, objects):
        workbook.set_properties({
            'comments': 'Created with Python and XlsxWriter from Odoo 11.0'})
        sheet = workbook.add_worksheet(_('BOM Structure'))
        sheet.set_landscape()
        sheet.fit_to_pages(1, 0)
        sheet.set_zoom(80)
        sheet.set_column(0, 0, 40)
        sheet.set_column(1, 1, 60)
        sheet.set_column(2, 2, 20)
        sheet.set_column(3, 3, 60)
        sheet.set_column(4, 4, 40)
        sheet.set_column(5, 8, 15)
        bold = workbook.add_format({'bold': True})
        title_style = workbook.add_format({'bold': True,
                                           'bg_color': '#FFFFCC',
                                           'bottom': 1})
        sheet_title = [_('BOM Name'),
                       _('Parent BOM'),
                       _('Product Reference'),
                       _('Product Name'),
                       _('Supplier'),
                       _('Quantity'),
                       _('Total Quantity'),
                       _('Unit Cost'),
                       _('Cost')
                       ]
        sheet.set_row(0, None, None, {'collapsed': 1})
        sheet.write_row(1, 0, sheet_title, title_style)
        sheet.freeze_panes(2, 0)
        i = 2
        for o in objects:
            cost = 0.0
            sheet.write(i, 0, o.product_tmpl_id.name or '', bold)
            sheet.write(i, 2, o.product_id.default_code or '', bold)
            sheet.write(i, 3, o.product_id.name or '', bold)
            sheet.write(i, 5, o.product_qty or '', bold)
            sheet.write(i, 6, o.product_qty or '', bold)
            i += 1
            j = 0
            for ch in o.bom_line_ids:
                i, cost = self.print_bom_children(workbook, ch, sheet, i, j, cost, 1, o.product_qty, o.product_id.name)
            i += 1
            sheet.write(i, 7, 'total cost', bold)
            sheet.write(i, 8, '$' + str(round(cost, 2)), bold)
            i += 2
