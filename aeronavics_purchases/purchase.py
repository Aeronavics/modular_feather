from odoo import models, fields, api

class purchaseInherit(models.Model):
    _inherit = 'product.supplierinfo'
    
    part_url = fields.Char(string='Product URL')
    uom = fields.Integer(string='UOM')
    uom_cost = fields.Float(string='UOM Cost')
